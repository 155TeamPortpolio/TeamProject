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

bool CResourceEntry::Begin_LoadAsync(const LoaderFunc& loaderFunc)
{
    LoadState expected = LoadState::Unloaded;
    //m_State가 지금 expected 값이면 Loading으로 바꾸고 성공(true).
    //아니면 바꾸지 않고 실패(false)하면서 expected를 “현재 값”으로 갱신.
    if (!m_State.compare_exchange_strong(expected, LoadState::Loading))
        return false;

    string capturedPath;
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        capturedPath = m_SourcePath;
        m_LastErrorMsg.clear();
        m_LoadingTask = shared_future<ResourceVariant>{};
    }
    //함수 하나를 다른 실행 컨텍스트에서 실행하고 결과를 future로 받는” 표준 기능launch::async를 줬기 때문에 새 스레드(또는 스레드풀)에서 즉시 실행
    future<ResourceVariant> futureTask = async(launch::async, [this, loaderFunc, capturedPath]()
        {
            string errorMsg;
            ResourceVariant loadedVariant = loaderFunc(capturedPath, errorMsg);

            if (!errorMsg.empty()) {
                lock_guard<mutex> lockGuard(m_Mutex);
                m_LastErrorMsg = errorMsg;
            }
            return loadedVariant;
        });

    {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_LoadingTask = futureTask.share();
    }

    return true;
}

void CResourceEntry::Pump(const CommitFunc& commitFunc)
{
    LoadState currentState = m_State.load(memory_order_acquire);
    if (currentState == LoadState::Unloaded || currentState == LoadState::Ready || currentState == LoadState::Failed)
        return;

    if (currentState == LoadState::Loading)
    {
        shared_future<ResourceVariant> capturedTask;
        {
            lock_guard<mutex> lockGuard(m_Mutex);
            capturedTask = m_LoadingTask;
        }

        if (!capturedTask.valid()) {
            m_State.store(LoadState::Failed, memory_order_release);
            return;
        }

        if (capturedTask.wait_for(chrono::seconds(0)) != future_status::ready)
            return;

        ResourceVariant loadedVariant = capturedTask.get();
        {
            lock_guard<mutex> lockGuard(m_Mutex);
            // 로딩 결과 임시 보관(아직 엔트리 소유로 AddRef 확정은 Commit 후)
            m_StagingResource = loadedVariant;
        }

        m_State.store(LoadState::Committing, memory_order_release);
        currentState = LoadState::Committing;
    }

    if (currentState == LoadState::Committing)
    {
        ResourceVariant stagingVariant;
        {
            lock_guard<mutex> lockGuard(m_Mutex);
            stagingVariant = m_StagingResource;
        }

        // monostate면 실패 처리
        if (holds_alternative<monostate>(stagingVariant)) {
            m_State.store(LoadState::Failed, memory_order_release);
            return;
        }

        string commitError;
        bool commitOk = true;
        if (commitFunc) {
            commitOk = commitFunc(stagingVariant, commitError);
        }

        if (!commitOk) {
            lock_guard<mutex> lockGuard(m_Mutex);
            m_LastErrorMsg = commitError;
            // stagingVariant가 들고 있는 raw ptr은 로더가 만든 것이므로 여기서 정리할 정책 필요
            // 간단히: 아래 ReleaseVariant로 해제
            ReleaseVariant_NoLock(stagingVariant);
            m_StagingResource = monostate{};
            m_State.store(LoadState::Failed, memory_order_release);
            return;
        }

        // Commit 성공: 엔트리 본 소유로 확정(엔트리 1회 AddRef 확보)
        {
            lock_guard<mutex> lockGuard(m_Mutex);

            Release_NoLock(); // 기존 리소스 정리

            AddRefVariant_NoLock(stagingVariant); // 엔트리 소유 1회 AddRef
            m_Resource = stagingVariant;

            m_StagingResource = monostate{};
            m_LoadingTask = shared_future<ResourceVariant>{};
        }

        m_State.store(LoadState::Ready, memory_order_release);
    }
}

void CResourceEntry::Reset()
{
    lock_guard<mutex> lockGuard(m_Mutex);
    Release_NoLock();
    ReleaseVariant_NoLock(m_StagingResource);
    m_StagingResource = std::monostate{};
    m_LoadingTask = shared_future<ResourceVariant>{};
    m_LastErrorMsg.clear();
    m_State.store(LoadState::Unloaded, std::memory_order_release);
}

void CResourceEntry::Release_NoLock()
{
    /*Variant 안에 값을 꺼내서 람다에 넣는 함수*/
   visit([&](auto&& value)
        {
           /*타입을 꺼내서 저장*/
            using Value = decay_t<decltype(value)>;
            /*포인터라면*/
            if constexpr (is_pointer_v<Value>)
            {
                Safe_Release(value);   
            }
        }, m_Resource);

    m_Resource = monostate{};
}

CResourceEntry* CResourceEntry::Create() {
	return new CResourceEntry;
}

void CResourceEntry::Free() {
        Reset();
        __super::Free();
}