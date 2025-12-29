#include "Engine_Defines.h"
#include "ThreadPool.h"

CThreadPool::CThreadPool(_uint threadCount)
{
    if (threadCount == 0) 
        threadCount = 1;

    m_Workers.reserve(threadCount);

    for (_uint i = 0; i < threadCount; ++i) {
        m_Workers.emplace_back([this] { WokerLoop(); });
    }
}

CThreadPool::~CThreadPool()
{
}

/*일 없으면 잠들고, 일 생기면 하나 꺼내서 실행하고, stop이면 종료*/
void CThreadPool::WokerLoop()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const _bool comInited = SUCCEEDED(hr);

    while (true)
    {
        function<void()> job;

        /*스코프를 통해서 자동으로 유니크 락/언락하려고*/
        {
            // wait() 때문에 unique_lock을 사용
            unique_lock<mutex> lock(m_mutex);
            //  뮤텍스의 잠금을 풀고, 스레드를 잠재우고 깨워지면 뮤텍스를 다시 잠그고
            //큐가 있거나, 중지 명령까지 잠들어야 함.
            /*조건이 만족될 때까지 재우는 함수 ->stop이 켜지거나, 큐에 작업이 생길 때까지 대기  */
            m_ConditionVariable.wait(lock, [this]  {return m_bStop.load(memory_order_acquire) || !m_Queues.empty();});

            //스탑 되고, 큐가 없으면 그만. ->
            if (m_bStop.load(memory_order_relaxed) && m_Queues.empty())
                return;

            // 큐에서 작업 하나 꺼내기
            job = move(m_Queues.front());
            m_Queues.pop_front();
            m_iActive.fetch_add(1,memory_order_acq_rel);
        } // 여기서 락 해제

        // 작업 실행 
        job();

        {
           lock_guard<mutex> lock(m_mutex);
            m_iActive.fetch_sub(1, memory_order_acq_rel); /*끝나면 하나 빼*/

            if (m_Queues.empty() && m_iActive.load(memory_order_acquire) == 0)
                m_idleConditionVariable.notify_all();
        }
    }
    if (comInited) CoUninitialize();
}

void CThreadPool::ShutDown()
{
    //이미 멈췄는지 한번 확인하는 것. 그러니까, 현재 저장되어 있는 값을 읽고, 그 값을 다른 값으로 치환.(즉, 폴스일 때 가져와서 트루로 바꿔둠) 
    //이러면 여러번 불러도 안전
    const _bool alreadyStopping = m_bStop.exchange(true, memory_order_acq_rel);//읽고 쓰는걸 둘다 한다는 플래그임
    if (alreadyStopping)
        return;

    // 잠들어 있는 워커 전부 깨워서 stop 조건을 보게 함 -> 왜냐면 아까 위에서 wait 조건을 설정했기 때문
    m_ConditionVariable.notify_all();

    // 워커 스레드 join
    for (auto& worker : m_Workers)
    {
        if (worker.joinable()) /*쓰레드 지금 사용 가능함? ()*/
            worker.join(); /*종료 기다림. 즉 워크 루프가 리턴 될때까지*/
    }

    m_Workers.clear();

    {/*다시 잠그고, 큐를 클리어함->왜냐면 이미 다 끝났을 테니까*/
        lock_guard<mutex> lock(m_mutex);
        m_Queues.clear();
    }
}

void CThreadPool::Wait_Idle()/*이건 워커가 아닌 호출자를 위한 것*/
{
    /*일단 쓰레드 잠그고*/
    unique_lock<mutex> lock(m_mutex);

    // 큐가 비고 + 실행 중 job도 0이면 idle
    m_idleConditionVariable.wait(lock, [this] {
        return m_Queues.empty() &&  (m_iActive.load(memory_order_acquire) == 0);
        });
}

bool CThreadPool::isQueueEmpty() const
{
    lock_guard<mutex> lock(m_mutex);
    return m_Queues.empty();
}

CThreadPool* CThreadPool::Create(_uint threadCount)
{
    return new CThreadPool(threadCount);
}

void CThreadPool::Free()
{
    ShutDown();
}
