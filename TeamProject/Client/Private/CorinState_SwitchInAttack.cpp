#include "pch.h"
#include "CorinState_SwitchInAttack.h"

#include "Corin.h"

void CCorinState_SwitchInAttack::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Start", CCorinState_SwitchInAttack_Start::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_SwitchInAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    __super::Enter(pOwner);
}

void CCorinState_SwitchInAttack::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CCorin>* pRootFSM = pOwner->Get_StateMachine();
        pRootFSM->Set_Trigger("ToIdle");
    }
}

void CCorinState_SwitchInAttack_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_02")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInAttack_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_SwitchInAttack_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_02_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInAttack_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    IHState<CCorin>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
