#pragma once
#include "Base.h"
#include <atomic>                           // m_stop, m_active 같은 원자 변수
#include <condition_variable>     // 작업 대기/깨우기
#include <deque>                           // 작업 큐 (앞에서 pop)
#include <future>                           // std::future / std::packaged_task
#include <mutex>                            // 뮤텍스
#include <thread>                           // 스레드 생성/join
#include <type_traits>                  // std::invoke_result_t

NS_BEGIN(Engine)
class CThreadPool :
    public CBase
{
private:
    explicit CThreadPool(_uint threadCount = thread::hardware_concurrency()); //하드웨어가 사용할 수 있는 스레드 수 반환하는 함수
    ~CThreadPool();

    CThreadPool(const CThreadPool&) = delete;
    CThreadPool& operator=(const CThreadPool&) = delete;

private:
    void WokerLoop();
    void ShutDown();
    void Wait_Idle();
    bool isQueueEmpty() const;

private:
    template <class Function, class... Args>/*후행 반환 타입 : Function을 args로 호출 했을 때 나오는 타입을 후행으로 선언하는 문법*/
    auto enqueue(Function&& f, Args&&... args)->future<invoke_result_t<Function, Args...>>;

private:
    atomic<_bool> m_bStop;
    atomic<_int>  m_iActive;
    mutable mutex m_mutex; /*잠금 -> 콘스트 떔시*/
    condition_variable m_ConditionVariable; /*현재 상태*/
    deque<function<void()>> m_Queues; /*작업 큐*/
    vector<thread> m_Workers; /*쓰레드 풀*/

    /*Idle 대기용*/
    mutex m_idleMutex;
    condition_variable m_idleConditionVariable;

public:
    static CThreadPool* Create();
    virtual void Free() override;
};
NS_END

template<class Function, class ...Args>
inline auto CThreadPool::enqueue(Function&& func, Args && ...args) -> future<invoke_result_t<Function, Args ...>>
{
    //invoke_result_t 후행 반환 타입 -> 템플릿 타입 반환값이 복잡할 때 사용
    using Return =invoke_result_t<Function, Args...>;



    /*packaged_task :  Return을 돌려주는 함수 객체를 저장해줌 -> 여기서 Return()은 호출이 아닌 타입임.*/
    auto task = make_shared<packaged_task<Return()>>(
        bind(forward<Function>(func),forward<Args>(args)...)
        // 바인드 -> 함수의 매개인자의 값을 미리 고정시켜 두는 것임(미리 뭐가 들어갈지 박아둔다는 뜻-> 나중에 그냥()로 호출 가능함)
        /*위 패키지에서 리턴()로  함수의 타입을 정했기 떄문에 바인드를 통해서 매개인자를 고정해두는 것임.*/
    );

    // task의 타입 : shared_ptr<packaged_task<Return()>>;
    //future :나중에 완료될 작업의 결과를 받는 핸들
    //auto fut = pool.enqueue([] { return 123; });
   // int v = fut.get();   // 작업이 끝나면 이 퓨처에 값이 저장된다고 봐야 함.
    future<Return>fut = task->get_future();

    {/*일부러 블럭을 만들어서, 블럭 스코프가 벗어나면 자동 언락됨*/
        lock_guard<mutex> lock(m_mutex);
        //load(emory_order_acquire) 읽고 쓰기 전에, 먼저 이 함수부터 실행을 한다.
        //즉 스탑인지 아닌지 먼저 읽겠다는 뜻 그 다음에 할거면 뭘 해라 라는 뜻.
        if (m_bStop.load(std::memory_order_relaxed))
            throw std::runtime_error("ThreadPool is stopping; cannot enqueue.");

        m_Queues.emplace_back([task]() { (*task)(); });
    }

    m_ConditionVariable.notify_one(); /*쓰레드 깨워!*/
    return fut;
}

/*Mutex*/
//동시에 건드리면 망가질 수 있는 공유 데이터를, 한 번에 한 스레드만 접근하게 막아주는 “잠금 장치”
/*condition_variable*/
//필수적인 것은 아니지만, 워커 쓰레드가 할 일이 없을 떄 계속 포문을 돌면서 락언락을 할 필요 없이, 재웠다가 꺠우는 역할임