#include "pch.h"
#include "CorinState_SwitchIn.h"

#include "Character.h"
#include "Corin.h"
#include "CorinState_SwitchInNormal.h"
#include "CorinState_SwitchInAttack.h"
#include "CorinState_SwitchInParryAid.h"

CCorinState_SwitchIn* CCorinState_SwitchIn::Create()
{
    auto pInstance = new CCorinState_SwitchIn();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Normal", CCorinState_SwitchInNormal::Create());
    pSubStateMachine->Register_State("Attack", CCorinState_SwitchInAttack::Create());
    pSubStateMachine->Register_State("ParryAid", CCorinState_SwitchInParryAid::Create());

    pSubStateMachine->Get_State("Normal")->Set_Tag("Normal");
    pSubStateMachine->Get_State("Attack")->Set_Tag("Attack");
    pSubStateMachine->Get_State("ParryAid")->Set_Tag("ParryAid");

    return pInstance;
}

void CCorinState_SwitchIn::Enter(CCorin* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Unlock_Move();

    switch (pOwner->Get_Switch())
    {
    case CCharacter::SWITCH::NORMAL:
        m_pSubStateMachine->Set_DefaultState("Normal");
        break;
    case CCharacter::SWITCH::ATTACK:
        m_pSubStateMachine->Set_DefaultState("Attack");
        break;
    case CCharacter::SWITCH::PARRYAID:
        m_pSubStateMachine->Set_DefaultState("ParryAid");
        break;
    }

    m_pSubStateMachine->Reset_Trigger("Complete");
    m_pSubStateMachine->Set_Int("ExitMode", 0);

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
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

_bool CCorinState_SwitchIn::Handle_Transition(CCorin* pOwner, const string& strState)
{
    if (m_pSubStateMachine->Get_CurrentStateName() == "ParryAid")
    {
        IHState<CCorin>* pState = dynamic_cast<IHState<CCorin>*>(m_pSubStateMachine->Get_CurrentState());
        if (pState->Get_SubStateMachine()->Get_CurrentState()->Get_Tag() != "End")
            return false;
    }
    return true;
}
