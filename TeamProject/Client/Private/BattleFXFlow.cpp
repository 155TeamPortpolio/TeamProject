#include "pch.h"
#include "BattleFXFlow.h"

CBattleFXFlow::CBattleFXFlow() {

}

void CBattleFXFlow::Start(function<void()> onEnd)
{
    m_onEnd = move(onEnd);
    m_stepIndex = 0;
    m_timeInStep = 0.f;
    m_isRunning = !m_steps.empty();
}
void CBattleFXFlow::Update(_float dt)
{
    if (!m_isRunning || m_steps.empty())
        return;

    while (m_isRunning && m_stepIndex < m_steps.size())
    {
        bool keep = m_steps[m_stepIndex](dt);
        if (keep) break;

        // step 종료 -> 다음 step
        m_stepIndex++;
        m_timeInStep = 0.f;

        if (m_stepIndex >= m_steps.size())
        {
            m_isRunning = false;
            if (m_onEnd) m_onEnd();
            break;
        }
    }
}

void CBattleFXFlow::Clear(_bool callOnEnd)
{
    if (callOnEnd && m_isRunning && m_onEnd)
        m_onEnd();

    m_steps.clear();
    m_stepIndex = 0;
    m_isRunning = false;
    m_timeInStep = 0.f;
    m_onEnd = {};
}

/*N초 기다림*/
void CBattleFXFlow::AddWait(_float duration)
{
    _float elapsed = 0.f;
    m_steps.emplace_back([=](_float dt) mutable -> bool {
        elapsed += dt;
        return elapsed < duration;
        });
}

/*1회 호출 후 넘어감*/
void CBattleFXFlow::AddCall(function<void()> fn)
{
    m_steps.emplace_back([fn = std::move(fn)](float) mutable -> bool {
        fn();
        return false; 
     });
}

CBattleFXFlow* CBattleFXFlow::Create()
{
	CBattleFXFlow* instance = new CBattleFXFlow();
	return instance;
}

void CBattleFXFlow::Free()
{
	__super::Free();
}
