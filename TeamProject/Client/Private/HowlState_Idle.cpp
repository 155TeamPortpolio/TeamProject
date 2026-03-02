#include "pch.h"
#include "HowlState_Idle.h"
#include "Howl.h"

void CHowlState_Idle::Enter(CHowl* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CHowl>::Create();
        m_pSubStateMachine->Register_State("Loop", CHowlState_Idle_Loop::Create());
        m_pSubStateMachine->Register_State("Once", CHowlState_Idle_Once::Create());

        m_pSubStateMachine->Register_Transition("Loop", "Once",
            CStateMachine<CHowl>::CONDITION_TIME_GREATER, "", 15.f);
        m_pSubStateMachine->Register_Transition("Once", "Loop",
            CStateMachine<CHowl>::CONDITION_ANIMATION_END);
    }

    m_pSubStateMachine->Set_DefaultState("Loop");

    __super::Enter(pOwner);
}

void CHowlState_Idle::Update(CHowl* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CHowlState_Idle::Exit(CHowl* pOwner)
{
    __super::Exit(pOwner);
}

void CHowlState_Idle_Loop::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle01")
        .Loop(true)
        .Apply();
}

void CHowlState_Idle_Once::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle03")
        .Loop(false)
        .Apply();
}