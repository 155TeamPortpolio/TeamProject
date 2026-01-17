#include "pch.h"
#include "JaneDoeState_SwitchInAttack.h"

#include "JaneDoe.h"

void CJaneDoeState_SwitchInAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Start", CJaneDoeState_SwitchInAttack_Start::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_SwitchInAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchInAttack::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CJaneDoe>* pRootFSM = pOwner->Get_StateMachine();
        pRootFSM->Set_Trigger("ToIdle");
    }
}

void CJaneDoeState_SwitchInAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CJaneDoeState_SwitchInAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    IHState<CJaneDoe>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
