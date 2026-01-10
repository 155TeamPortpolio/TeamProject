#include "pch.h"
#include "JaneDoeState_SwitchInExAttack.h"

#include  "JaneDoe.h"

void CJaneDoeState_SwitchInExAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Start", CJaneDoeState_SwitchInExAttack_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CJaneDoeState_SwitchInExAttack_Loop::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_SwitchInExAttack_End::Create());

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchInExAttack::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CJaneDoe>* pRootFSM = pOwner->Get_StateMachine();
        pRootFSM->Set_Trigger("ToIdle");
    }
}

void CJaneDoeState_SwitchInExAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_Start")
        .Loop(false)
        .Speed(1.2f)
        .Apply();    
}

void CJaneDoeState_SwitchInExAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CJaneDoeState_SwitchInExAttack_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInExAttack_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CJaneDoeState_SwitchInExAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInExAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}