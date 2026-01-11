#include "pch.h"
#include "BangBooAskState_Idle.h"

#include "BangBooAsk.h"

void CBangBooAskState_Idle::Enter(CBangBooAsk* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBangBooAsk>::Create();
        m_pSubStateMachine->Register_State("Start", CBangBooAskState_Idle_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CBangBooAskState_Idle_Loop::Create());

        m_pSubStateMachine->Get_State("Loop")->Set_Tag("Loop");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CBangBooAsk>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CBangBooAskState_Idle::Update(CBangBooAsk* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CBangBooAskState_Idle_Start::Enter(CBangBooAsk* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle_Start")
        .Loop(false)
        .Apply();
}

void CBangBooAskState_Idle_Loop::Enter(CBangBooAsk* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle_Loop")
        .Loop(true)
        .Apply();
}
