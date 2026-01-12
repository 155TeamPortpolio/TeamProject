#include "pch.h"
#include "CorinState_SwitchIn.h"

#include "Character.h"
#include "Corin.h"
#include "CorinState_SwitchInNormal.h"
//#include "CorinState_SwitchInAttack.h"
//#include "CorinState_SwitchInExAttack.h"
//#include "CorinState_SwitchInParryAid.h"

void CCorinState_SwitchIn::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Normal", CCorinState_SwitchInNormal::Create());
        //m_pSubStateMachine->Register_State("Attack", CCorinState_SwitchInAttack::Create());
        //m_pSubStateMachine->Register_State("ExAttack", CCorinState_SwitchInExAttack::Create());
        //m_pSubStateMachine->Register_State("ParryAid", CCorinState_SwitchInParryAid::Create());

        m_pSubStateMachine->Get_State("Normal")->Set_Tag("Normal");
        //m_pSubStateMachine->Get_State("Attack")->Set_Tag("Attack");
        //m_pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
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

void CCorinState_SwitchIn::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");

        _int iExitMode = m_pSubStateMachine->Get_Int("ExitMode");
        m_pSubStateMachine->Set_Int("ExitMode", 0);

        CStateMachine<CCorin>* pRootFSM = pOwner->Get_StateMachine();

        switch (iExitMode)
        {
        case 1:
            pRootFSM->Set_Trigger("ToMove");
            break;
        case 2:
            pRootFSM->Set_Trigger("Attack");
            break;
        case 3:
            pRootFSM->Set_Trigger("ToEvade");
            break;
        default:
            pRootFSM->Set_Trigger("ToIdle");
            break;
        }
    }
}

void CCorinState_SwitchIn::Exit(CCorin* pOwner)
{
}
