#include "pch.h"
#include "HowlState_Wake.h"
#include "Howl.h"

void CHowlState_Wake::Enter(CHowl* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CHowl>::Create();

        m_pSubStateMachine->Register_State("Start", CHowlState_Wake_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CHowlState_Wake_Loop::Create());
        m_pSubStateMachine->Register_State("End", CHowlState_Wake_End::Create());
        m_pSubStateMachine->Register_State("Idle", CHowlState_Wake_Idle::Create());

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CHowl>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CHowl>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("End", "Idle",
            CStateMachine<CHowl>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
}

void CHowlState_Wake::Update(CHowl* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CHowlState_Wake::Exit(CHowl* pOwner)
{
    __super::Exit(pOwner);
}

void CHowlState_Wake_Start::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle02_Start")
        .Loop(false)
        .Apply();
}

void CHowlState_Wake_Loop::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle02_Loop")
        .Loop(false)
        .Apply();
}

void CHowlState_Wake_End::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle02_End")
        .Loop(false)
        .Apply();
}

void CHowlState_Wake_Idle::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle05")
        .Loop(true)
        .Apply();
}