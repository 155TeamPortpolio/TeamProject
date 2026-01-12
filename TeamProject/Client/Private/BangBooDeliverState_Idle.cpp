#include "pch.h"
#include "BangBooDeliverState_Idle.h"

#include "BangBooDeliver.h"

void CBangBooDeliverState_Idle::Enter(CBangBooDeliver* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBangBooDeliver>::Create();
        m_pSubStateMachine->Register_State("Start", CBangBooDeliverState_Idle_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CBangBooDeliverState_Idle_Loop::Create());

        m_pSubStateMachine->Get_State("Loop")->Set_Tag("Loop");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CBangBooDeliver>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CBangBooDeliverState_Idle::Update(CBangBooDeliver* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CBangBooDeliverState_Idle_Start::Enter(CBangBooDeliver* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle_Start")
        .Loop(false)
        .Apply();
}

void CBangBooDeliverState_Idle_Loop::Enter(CBangBooDeliver* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle_Loop")
        .Loop(true)
        .Apply();
}
