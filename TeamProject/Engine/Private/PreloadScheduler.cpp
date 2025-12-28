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

        m_tasks.emplace(requestKey,move(task));
        ++m_total;
    }

    auto loadFunction = [this, requestKey]() -> _bool
        {
            SetState(requestKey, PreloadState::Loading, "");

            _bool success = false;
            string errorMessage;
            {
                lock_guard<mutex> lockGuard(m_loaderMutex);
                auto iteratorLoader = m_loaders.find(requestKey.type);
                if (iteratorLoader == m_loaders.end())
                {
                    errorMessage = "No loader registered for type.";
                }
                else
                {
                    success = iteratorLoader->second(requestKey, errorMessage);
                }
            }

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
        if (iteratorTask != m_tasks.end())
            iteratorTask->second.future = move(jobFuture);
    }

    return true;
}

void CPreloadScheduler::Pump(vector<PreloadCompleted>& outCompleted)
{
    outCompleted.clear();

    lock_guard<mutex> lockGuard(m_mutex);

    for (auto it = m_tasks.begin(); it != m_tasks.end(); )
    {
        PreloadTask& task = it->second;

        if (!task.future.valid())
        {
            ++it; continue;
        }

        if (task.future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        {
            ++it; continue;
        }

        const _bool success = task.future.get();
        task.state = success ? PreloadState::Ready : PreloadState::Failed;

        PreloadCompleted completed{};
        completed.key = task.key;
        completed.state = task.state;
        completed.errorMessage = task.errorMessage;
        outCompleted.push_back(move(completed));
        ++m_done;

        it = m_tasks.erase(it);
    }
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
