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
    
    /*움직임*/
    TraceFlag eTraceFlag = {};
    _bool LockTarget = {};
    _vector3 CurrentDir = _vector3(0.f, 0.f, 1.f); 

    /*패턴*/
    _int patternIndex = { 8 };
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


struct DefilerAttackType {
    string AtkBone    = {"Weapon"};
    string AtkEvade   = {"Evade" };
    string AtkPower   = {"Hard"  };
    _bool  OnOff      = { false  };
};

static const unordered_map<string, DefilerAttackType> DefilerAtkData = 
{
    {"A_Type_On",{"Weapon","Parry","Normal",true}}  ,{"A_Type_Off",{"Weapon","Parry","Normal",false}},
    {"B_Type_On",{"Weapon","Evade","Normal",true}}  ,{"B_Type_Off",{"Weapon","Evade","Normal",false}},
    {"C_Type_On",{"Weapon","Parry","Hard",true}}    ,{"C_Type_Off",{"Weapon","Parry","Hard",false}},
    {"D_Type_On",{"Weapon","Evade","Hard",true}}    ,{"D_Type_Off",{"Weapon","Evade","Hard",false}},
    {"E_Type_On",{"Tail","Parry","Normal",true}}    ,{"E_Type_Off",{"Tail","Parry","Normal",false}},
    {"F_Type_On",{"Tail","Evade","Normal",true}}    ,{"F_Type_Off",{"Tail","Evade","Normal",false}},
    {"G_Type_On",{"Tail","Parry","Hard",true}}      ,{"G_Type_Off",{"Tail","Parry","Hard",false}},
    {"H_Type_On",{"Tail","Evade","Hard",true}}      ,{"H_Type_Off",{"Tail","Evade","Hard",false}},
    {"I_Type_On",{"Area","Parry","Normal",true}}    ,{"I_Type_Off",{"Area","Parry","Normal",false}},
    {"J_Type_On",{"Area","Evade","Normal",true}}    ,{"J_Type_Off",{"Area","Evade","Normal",false}},
    {"K_Type_On",{"Area","Parry","Hard",true}}      ,{"K_Type_Off",{"Area","Parry","Hard",false}},
    {"L_Type_On",{"Area","Evade","Hard",true}}      ,{"L_Type_Off",{"Area","Evade","Hard",false}},
};


struct DefilerDissolve {
    enum DISSOLVE_STATE { DISAPPEAR, APPEAR, NONE, END };
    DISSOLVE_STATE eDissolveState = NONE;
    _float fDissolveDuration = 2.f;
    _float fDissolveElapsedTime = 0.f;
    _float fDissolveProgress = 0.f;

    void Set_DissolveState(DISSOLVE_STATE state, _float duration)
    {
        eDissolveState = state;
        fDissolveDuration = duration;
        fDissolveElapsedTime = 0.f;
        fDissolveProgress = 0.f;
    }
    _bool isComplete() {
        return fDissolveElapsedTime >= fDissolveDuration;
    }
};