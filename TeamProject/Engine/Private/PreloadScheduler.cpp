#include "Engine_Defines.h"
#include "PreloadScheduler.h"
#include "ThreadPool.h"

CPreloadScheduler::CPreloadScheduler(CThreadPool* pThreadPool)
    :m_threadPool(pThreadPool)
{
}

CPreloadScheduler::~CPreloadScheduler()
{
}

_bool CPreloadScheduler::Request(const PreloadKey& requestKey)
{
    {
        lock_guard<mutex> lockGuard(m_mutex);
        if (m_tasks.find(requestKey) != m_tasks.end())
            return false; /*이미 요청된 작업임*/

        PreloadTask task{};
        task.key = requestKey;
        task.priority = requestKey.options.priority;
        task.state = PreloadState::Queued;
        task.startTime = std::chrono::steady_clock::now(); 
        m_tasks.emplace(requestKey,move(task));
        ++m_total;
    }

    auto loadFunction = [this, requestKey]() -> _bool
        {
            {
                lock_guard<mutex> lockGuard(m_mutex);
                auto itTask = m_tasks.find(requestKey);
                if (itTask != m_tasks.end())
                    itTask->second.loadingTime = std::chrono::steady_clock::now();
            }

            SetState(requestKey, PreloadState::Loading, "");

            string errorMessage;
            LoaderFunc loaderFunc;
            {
                lock_guard<mutex> lockGuard(m_loaderMutex);
                auto it = m_loaders.find(requestKey.type);
                if (it == m_loaders.end())
                {
                    SetError(requestKey, "No loader registered for type.");
                    return false;
                }
                loaderFunc = it->second; // 복사
            } // 여기서 m_loaderMutex 해제

            // 락 없는 상태에서 실제 로딩 실행
            _bool success = loaderFunc(requestKey, errorMessage);


            if (!success && errorMessage.empty())
                errorMessage = "Load failed.";

            if (!success)
                SetError(requestKey, errorMessage);

            return success;
        };

    future<_bool> jobFuture = m_threadPool->enqueue(move(loadFunction));
    
    {
        lock_guard<mutex> lockGuard(m_mutex);
        auto iteratorTask = m_tasks.find(requestKey);
        if (iteratorTask != m_tasks.end()) {
            iteratorTask->second.future = move(jobFuture);
        }
    }

    return true;
}

void CPreloadScheduler::Pump(vector<PreloadCompleted>& outCompleted)
{
    outCompleted.clear();

    size_t completedCount = 0;

    for (;;)
    {
        if (completedCount >= kMaxCompletePerPump)
            break;

        PreloadKey doneKey{};
        future<_bool> doneFuture;
        string doneError;
        bool found = false;

        { // m_mutex scope
            lock_guard<mutex> lockGuard(m_mutex);

            const auto now = std::chrono::steady_clock::now();
            size_t scanned = 0;

            for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it)
            {
                if (scanned++ >= kMaxScanPerPump)
                    break;

                PreloadTask& task = it->second;

                if (!task.future.valid())
                    continue;

                if (task.future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    // (선택) 병목 체크 로깅은 여기
                    // const auto queuedMs = ...
                    // const bool started = ...
                    continue;
                }

                doneKey = task.key;
                doneError = task.errorMessage;
                doneFuture = std::move(task.future);
                m_tasks.erase(it);
                found = true;
                break;
            }
        } // m_mutex 해제

        if (!found)
            break;

        const _bool success = doneFuture.get();

        PreloadCompleted completed{};
        completed.key = doneKey;
        completed.state = success ? PreloadState::Ready : PreloadState::Failed;
        completed.errorMessage = doneError;

        outCompleted.push_back(std::move(completed));
        ++m_done;
        ++completedCount;
    }
}



_bool CPreloadScheduler::IsAllDone() const
{
    const _uint total = m_total.load();
    if (total == 0) return true; 
    return (m_done.load() >= total);
}


void CPreloadScheduler::GetProgress(_uint& outDone, _uint& outTotal)
{
    outDone = m_done.load();
    outTotal = m_total.load();
}

PreloadState CPreloadScheduler::GetState(const PreloadKey& requestKey) const
{
    lock_guard<std::mutex> lockGuard(m_mutex);
    auto iteratorTask = m_tasks.find(requestKey);
    if (iteratorTask == m_tasks.end())
        return PreloadState::Failed; 
    return iteratorTask->second.state;
}

void CPreloadScheduler::BindLoader(ResourceType type, LoaderFunc loader)
{
    lock_guard<mutex> lockGuard(m_loaderMutex);
    m_loaders[type] = move(loader);
}

void CPreloadScheduler::Reset()
{
     lock_guard<mutex> lockGuard(m_mutex);
    m_tasks.clear();
    m_done.store(0);
    m_total.store(0);
}

void CPreloadScheduler::SetState(const PreloadKey& requestKey, PreloadState state, const std::string& errorMessage)
{
   lock_guard<mutex> lockGuard(m_mutex);
    auto iteratorTask = m_tasks.find(requestKey);
    if (iteratorTask == m_tasks.end())
        return;
    iteratorTask->second.state = state;
    if (!errorMessage.empty())
        iteratorTask->second.errorMessage = errorMessage;
}

void CPreloadScheduler::SetError(const PreloadKey& requestKey, const std::string& errorMessage)
{
    lock_guard<mutex> lockGuard(m_mutex);
    auto iteratorTask = m_tasks.find(requestKey);
    if (iteratorTask == m_tasks.end())
        return;
    iteratorTask->second.errorMessage = errorMessage;
}

CPreloadScheduler* CPreloadScheduler::Create(CThreadPool* pThreadPool)
{
    return new CPreloadScheduler(pThreadPool);
}

void CPreloadScheduler::Free()
{
    Safe_Release(m_threadPool);
    __super::Free();
}
