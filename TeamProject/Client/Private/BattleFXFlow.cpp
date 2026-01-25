#include "pch.h"
#include "BattleFXFlow.h"
#include "Engine_Math.h"

CBattleFXFlow::CBattleFXFlow() {

}

void CBattleFXFlow::Start(function<void()> onEnd)
{
    m_onEnd = move(onEnd);
    m_stepIndex = 0;
    m_timeInStep = 0.f;
    m_isRunning = !m_steps.empty() || !m_parallelTracks.empty();

    for (auto& track : m_parallelTracks)
        track.isRunning = !track.steps.empty();
}

void CBattleFXFlow::Update(_float dt)
{
    if (!m_isRunning)
        return;

    /*병렬 트랙*/
    for (_int trackIndex = 0; trackIndex < m_parallelTracks.size(); )
    {
        auto& track = m_parallelTracks[trackIndex];
        track.elapsed += dt;

        while (track.isRunning && track.stepIndex < track.steps.size())
        {
            bool keep = track.steps[track.stepIndex](dt);
            if (keep) break;

            track.stepIndex++;
            if (track.stepIndex >= track.steps.size())
                track.isRunning = false;
        }

        const _bool timeOver = (track.elapsed >= track.duration);

        if (timeOver)
        {
            if (track.onEnd) track.onEnd();
            m_parallelTracks.erase(m_parallelTracks.begin() + trackIndex);
            continue;
        }

        ++trackIndex;
    }

    // 메인 시퀀스 진행
    while (m_stepIndex < m_steps.size())
    {
        _bool keep = m_steps[m_stepIndex](dt);
        if (keep) break;

        m_stepIndex++;
        if (m_stepIndex >= m_steps.size())
            break;
    }

    const _bool mainDone = (m_stepIndex >= m_steps.size());
    const _bool hasParallel = !m_parallelTracks.empty();

    if (mainDone && !hasParallel)
    {
        m_isRunning = false;
        if (m_onEnd) m_onEnd();
        m_onEnd = {};
    }
    else
    {
        m_isRunning = true;
    }
}

void CBattleFXFlow::Clear(_bool callOnEnd)
{
    if (callOnEnd && m_isRunning && m_onEnd)
        m_onEnd();

    m_steps.clear();
    m_parallelTracks.clear();
    m_stepIndex = 0;
    m_isRunning = false;
    m_timeInStep = 0.f;
    m_onEnd = {};
}

void CBattleFXFlow::Cancel()
{
    if (m_onCancel)
        m_onCancel();

    m_isRunning = false;
    m_stepIndex = 0;
    m_steps.clear();
    m_parallelTracks.clear();

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

void CBattleFXFlow::AddParallel(_float duration, function<void(CBattleFXFlow& subFlow)> build, VoidFunc onParallelEnd)
{
    ParallelTrack track;
    track.duration = duration;
    track.elapsed = 0.f;
    track.onEnd = move(onParallelEnd);

    /*값복사*/
    CBattleFXFlow tempFlow;
    if (build) build(tempFlow);

    track.steps = move(tempFlow.m_steps);
    track.stepIndex = 0;
    track.isRunning = !track.steps.empty();

    m_parallelTracks.emplace_back(move(track));
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
