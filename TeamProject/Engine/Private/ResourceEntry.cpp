#include "Engine_Defines.h"
#include "ResourceEntry.h"
#include "Texture.h"
#include "ModelData.h"
#include "MaterialData.h"
#include "SoundData.h"
#include "AnimationClip.h"
#include "Shader.h"

CResourceEntry::CResourceEntry()
{
}

/*비동기로딩 시작 로더 펑션은 무엇을, 스케줄 펑션은 어디에다가*/
bool CResourceEntry::Begin_LoadAsync(const LoaderFunc& loaderFunc, const ScheduleFunc& scheduleFunc)
{
    {/*일단 로드가 되었는지 판별*/
        LoadState expectedState = LoadState::Unloaded;
        if (!m_State.compare_exchange_strong(expectedState, LoadState::Loading)) /*안되었으면 로딩중으로 바꾸고 로드 시작*/
            return false;


        string capturedPath;
        uint64_t capturedGeneration = 0;

        /*락-*/
        {
            lock_guard<mutex> lockGuard(m_Mutex);
            capturedPath = m_SourcePath; /*소스 패스 넣어주기*/
            m_LastErrorMsg.clear();
            m_LoadingTask = shared_future<ResourceVariant>{};
            capturedGeneration = ++m_Generation;
        }

        m_LoadingTask = scheduleFunc([loaderFunc, capturedPath, capturedGeneration, this]() -> ResourceVariant
            {
                string errorMsgLocal;
                ResourceVariant loadedVariant = loaderFunc(capturedPath, errorMsgLocal);

                if (!errorMsgLocal.empty())
                {
                    lock_guard<mutex> lockGuard(m_Mutex);
                    // 세대가 바뀌었으면 Reset 
                    if (capturedGeneration == m_Generation)
                        m_LastErrorMsg = errorMsgLocal;
                }
                return loadedVariant;
            });

        return true;
    }
}

void CResourceEntry::Pump_CompletedOnly()
{
    // Loading이 아니면 할 게 없음
    if (m_State.load(memory_order_acquire) != LoadState::Loading)
        return;

    shared_future<ResourceVariant> loadingTaskCopy;
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        loadingTaskCopy = m_LoadingTask;
    }

    if (!loadingTaskCopy.valid())
        return;

    if (loadingTaskCopy.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        return;

    ResourceVariant loadedVariant = std::monostate{};
    string errorMsg;

    try
    {
        loadedVariant = loadingTaskCopy.get();
    }
    catch (const std::exception& exceptionValue)
    {
        errorMsg = exceptionValue.what();
        loadedVariant = std::monostate{};
    }
    catch (...)
    {
        errorMsg = "Unknown exception while loading resource.";
        loadedVariant = std::monostate{};
    }

    lock_guard<mutex> lockGuard(m_Mutex);

    // LoadingTask 정리
    m_LoadingTask = shared_future<ResourceVariant>{};

    // 기존 리소스 교체
    ReleaseVariant_NoLock(m_Resource);

    if (holds_alternative<std::monostate>(loadedVariant))
    {
        m_LastErrorMsg = errorMsg.empty() ? m_LastErrorMsg : errorMsg;
        m_Resource = std::monostate{};
        m_State.store(LoadState::Failed, memory_order_release);
        return;
    }

    m_Resource = loadedVariant;
    m_State.store(LoadState::Ready, memory_order_release);
}

void CResourceEntry::Pump()
{
    if (m_State.load(memory_order_acquire) != LoadState::Loading)
        return;

    shared_future<ResourceVariant> loadingTaskCopy;
    uint64_t currentGeneration = 0;
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        loadingTaskCopy = m_LoadingTask;
        currentGeneration = m_Generation;
    }

    if (!loadingTaskCopy.valid())
        return;

    if (loadingTaskCopy.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        return;

    ResourceVariant loadedVariant = loadingTaskCopy.get();

    lock_guard<mutex> lockGuard(m_Mutex);

    // Reset이 중간에 들어갔으면 결과 폐기
    if (currentGeneration != m_Generation)
    {
        ReleaseVariant_NoLock(loadedVariant);
        return;
    }

    // 이전 리소스 해제 후 교체
    ReleaseVariant_NoLock(m_Resource);

    if (holds_alternative<std::monostate>(loadedVariant))
    {
        m_State.store(LoadState::Failed, memory_order_release);
        // m_LastErrorMsg는 loader에서 써뒀거나, 여기서 세팅해도 됨
        m_Resource = std::monostate{};
    }
    else
    {
        m_Resource = loadedVariant; // 엔트리가 소유(Release는 Reset/Free에서)
        m_State.store(LoadState::Ready, memory_order_release);
    }
}

void CResourceEntry::Reset()
{
    // 메인 스레드에서만 호출
    lock_guard<mutex> lockGuard(m_Mutex);

    ++m_Generation;              // 진행중 작업 결과 무효화
    m_LastErrorMsg.clear();

    // 현재 보유 리소스 해제
    ReleaseVariant_NoLock(m_Resource);
    ReleaseVariant_NoLock(m_FallbackResource);

    // 로딩 작업 핸들 정리(진행중이어도 결과는 세대 체크로 버려짐)
    m_LoadingTask = shared_future<ResourceVariant>{};

    m_State.store(LoadState::Unloaded, memory_order_release);
}

void CResourceEntry::ForceSetReady(ResourceVariant resourceVariant)

{
    lock_guard<mutex> lockGuard(m_Mutex);

    // 기존 것 정리
    ReleaseVariant_NoLock(m_Resource);

    // 새 값 저장 + AddRef
    m_Resource = resourceVariant;
    AddRefVariant_NoLock(m_Resource);

    m_LastErrorMsg.clear();
    m_LoadingTask = std::shared_future<ResourceVariant>{};
    m_State.store(LoadState::Ready, std::memory_order_release);
}

void CResourceEntry::SetFallback(ResourceVariant fallbackVariant)
{
        std::lock_guard<std::mutex> lockGuard(m_Mutex);

        ReleaseVariant_NoLock(m_FallbackResource);
        m_FallbackResource = fallbackVariant;
        AddRefVariant_NoLock(m_FallbackResource);
}

CResourceEntry* CResourceEntry::Create() {
	return new CResourceEntry;
}

void CResourceEntry::Free() {
        shared_future<ResourceVariant> loadingTaskCopy;
        {
            lock_guard<mutex> lockGuard(m_Mutex);
            loadingTaskCopy = m_LoadingTask;
            m_LoadingTask = shared_future<ResourceVariant>{};
        }

        // future 결과 회수 + 리소스 Release (블록 가능)
        if (loadingTaskCopy.valid())
        {
            ResourceVariant loadedVariant = std::monostate{};

            try
            {
                loadedVariant = loadingTaskCopy.get();
            }
            catch (...)
            {
                loadedVariant = std::monostate{};
            }

            ReleaseVariant_NoLock(loadedVariant);
        }

        {
            std::lock_guard<std::mutex> lockGuard(m_Mutex);
            ReleaseVariant_NoLock(m_Resource);
            ReleaseVariant_NoLock(m_FallbackResource);
            m_LastErrorMsg.clear();
            m_SourcePath.clear();
            m_LevelTag.clear();
            m_DebugName.clear();
            m_State.store(LoadState::Unloaded, std::memory_order_release);
        }

        __super::Free();
}