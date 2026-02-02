#pragma once

#include "DefilerState_Born.h"
#include "DefilerState_Idle.h"
#include "DefilerState_Attack.h"
/*상태간 타겟 추적 모드*/
enum class TraceFlag : _uint
{
    None = 0,
    TrackTarget = 1 << 0,  // 타겟 방향으로 이동/회전
    StopAtTarget = 1 << 1,  // 타겟 도달 시 정지
    AllowThroughTarget = 1 << 2,  // 타겟 지나침 가능
    IgnoreTarget = 1 << 3,  // 타겟 존재 무시 (연출/패턴 고정)
};

inline TraceFlag operator|(TraceFlag a, TraceFlag b)
{
    return static_cast<TraceFlag>(
        static_cast<_uint>(a) | static_cast<_uint>(b));
}

inline TraceFlag operator&(TraceFlag a, TraceFlag b)
{
    return static_cast<TraceFlag>(
        static_cast<_uint>(a) & static_cast<_uint>(b));
}

inline bool HasFlag(TraceFlag value, TraceFlag flag)
{
    return (static_cast<_uint>(value) & static_cast<_uint>(flag)) != 0;
}

typedef struct tagDefilerBlackBoard
{
    _float3 vStartPos = {};
    _bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
    _bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부
    
    /*움직임 패턴*/
    TraceFlag eTraceFlag = {};
    _bool LockTarget = {};
    _bool RotateLock = {};
    _vector3 CurrentDir = _vector3(0.f, 0.f, 1.f); 

    /*패턴*/
    _int patternIndex = { 0 };
    struct DefilerPattern { string nextPattern; _float animStartProgress; _float animEndProgress;};
    deque<DefilerPattern> patternTransition;
    DefilerPattern reservedPattern = {};

    /*추적 상태 */
    _bool CompareTrace(TraceFlag flag) { return HasFlag(eTraceFlag, flag); }
    void TraceType_Fierce()   { ResetTraceFlag(); eTraceFlag = TraceFlag::TrackTarget | TraceFlag::AllowThroughTarget;};
    void TraceType_OnTarget() { ResetTraceFlag(); eTraceFlag = TraceFlag::TrackTarget | TraceFlag::StopAtTarget; };
    void TraceType_OnlyAnim() { ResetTraceFlag(); eTraceFlag = TraceFlag::IgnoreTarget; };

    /*전환 상태*/
    void ReservePattern() {
        reservedPattern = patternTransition.front();
        patternTransition.pop_front();
    }
    void ResetNextFlag() {
        isChainOpen = false;
        isRequestNext = false;
    }
    void ResetTraceFlag() {
        eTraceFlag =TraceFlag::None;
    }
}DEFILER_BLACK_BOARD;
