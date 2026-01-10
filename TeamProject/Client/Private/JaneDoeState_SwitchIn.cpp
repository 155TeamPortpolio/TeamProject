#include "pch.h"
#include "JaneDoeState_SwitchIn.h"

#include "JaneDoe.h"
#include "JaneDoe_SwitchInNormal.h"

void CJaneDoeState_SwitchIn::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Normal", CJaneDoe_SwitchInNormal::Create());
        //m_pSubStateMachine->Register_State("Attack", CJaneDoeState_BackStep::Create());
        //m_pSubStateMachine->Register_State("ExAttack", CJaneDoeState_BackStep::Create());
        //m_pSubStateMachine->Register_State("ParryAid", CJaneDoeState_BackStep::Create());

        m_pSubStateMachine->Get_State("Normal")->Set_Tag("Normal");
        //m_pSubStateMachine->Get_State("Attack")->Set_Tag("Attack");
        //m_pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
        //m_pSubStateMachine->Get_State("ParryAid")->Set_Tag("ParryAid");
    }

    //  switch(pOwner->GetSwitchType())
    // {
    // case Switch::Normal:
    //    m_pSubStateMachine->Set_DefaultState("Normal");
    // case Switch::Attack:
    //    m_pSubStateMachine->Set_DefaultState("Attack");
    // case Switch::ExAttack:
    //    m_pSubStateMachine->Set_DefaultState("ExAttack");
    // case Switch::ParryAid:
    //    m_pSubStateMachine->Set_DefaultState("ParryAid");
    // }
    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchIn::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        _int iExitMode = m_pSubStateMachine->Get_Int("ExitMode");
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CJaneDoe>* pRootFSM = pOwner->Get_StateMachine();

        switch (iExitMode)
        {
        case 4:
            pRootFSM->Set_Int("IdleEntryMode", 1);
            pRootFSM->Set_Trigger("ToIdle");
            break;
        case 3: // RushAttack
            pRootFSM->Set_Int("AttackEntryMode", 1);
            pRootFSM->Set_Trigger("Attack");
            break;
        case 2: // Run
            pRootFSM->Set_Int("MoveEntryMode", 2);
            pRootFSM->Set_Trigger("ToMove");
            break;
        default: // Idle
            pRootFSM->Set_Trigger("ToIdle");
            break;
        }
    }
}

void CJaneDoeState_SwitchIn::Exit(CJaneDoe* pOwner)
{
}
