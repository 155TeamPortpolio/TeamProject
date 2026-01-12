#include "pch.h"
#include "BangBooPayState_Idle.h"

#include "BangBooPay.h"

void CBangBooPayState_Idle::Enter(CBangBooPay* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBangBooPay>::Create();
        m_pSubStateMachine->Register_State("Start", CBangBooPayState_Idle_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CBangBooPayState_Idle_Loop::Create());

        m_pSubStateMachine->Get_State("Loop")->Set_Tag("Loop");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CBangBooPay>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CBangBooPayState_Idle::Update(CBangBooPay* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CBangBooPayState_Idle_Start::Enter(CBangBooPay* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle_Start")
        .Loop(false)
        .Apply();
}

void CBangBooPayState_Idle_Loop::Enter(CBangBooPay* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle_Loop")
        .Loop(true)
        .Apply();
}
