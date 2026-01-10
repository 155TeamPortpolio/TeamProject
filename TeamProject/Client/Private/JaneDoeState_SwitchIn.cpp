#include "pch.h"
#include "JaneDoeState_SwitchIn.h"

#include "Character.h"
#include "JaneDoe.h"
#include "JaneDoeState_SwitchInAttack.h"
#include "JaneDoeState_SwitchInExAttack.h"
#include "JaneDoeState_SwitchInNormal.h"

void CJaneDoeState_SwitchIn::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Normal", CJaneDoeState_SwitchInNormal::Create());
        m_pSubStateMachine->Register_State("Attack", CJaneDoeState_SwitchInAttack::Create());
        m_pSubStateMachine->Register_State("ExAttack", CJaneDoeState_SwitchInExAttack::Create());
        //m_pSubStateMachine->Register_State("ParryAid", CJaneDoeState_BackStep::Create());

        m_pSubStateMachine->Get_State("Normal")->Set_Tag("Normal");
        m_pSubStateMachine->Get_State("Attack")->Set_Tag("Attack");
        m_pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
        //m_pSubStateMachine->Get_State("ParryAid")->Set_Tag("ParryAid");
    }

    switch (pOwner->Get_Switch())
    {
    case CCharacter::SWITCH::NORMAL:
        m_pSubStateMachine->Set_DefaultState("Normal");
        break;
    case CCharacter::SWITCH::ATTACK:
        m_pSubStateMachine->Set_DefaultState("Attack");
        break;
    case CCharacter::SWITCH::EXATTACK:
        m_pSubStateMachine->Set_DefaultState("ExAttack");
        break;
    case CCharacter::SWITCH::PARRYAID:
        m_pSubStateMachine->Set_DefaultState("ParryAid");
        break;
    }
    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchIn::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CJaneDoe>* pRootFSM = pOwner->Get_StateMachine();
        pRootFSM->Set_Trigger("ToIdle");
    }
}

void CJaneDoeState_SwitchIn::Exit(CJaneDoe* pOwner)
{
}
