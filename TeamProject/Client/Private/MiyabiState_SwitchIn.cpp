#include "pch.h"
#include "MiyabiState_SwitchIn.h"

#include "Character.h"
#include "Miyabi.h"
#include "MiyabiState_SwitchInNormal.h"
#include "MiyabiState_SwitchInAttack.h"
#include "MiyabiState_SwitchInParryAid.h"

CMiyabiState_SwitchIn* CMiyabiState_SwitchIn::Create()
{
    auto pInstance = new CMiyabiState_SwitchIn();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInNormal", CMiyabiState_SwitchInNormal::Create());
    pSubStateMachine->Register_State("SwitchInAttack", CMiyabiState_SwitchInAttack::Create());
    pSubStateMachine->Register_State("SwitchInParryAid", CMiyabiState_SwitchInParryAid::Create());

    pSubStateMachine->Get_State("SwitchInNormal")->Set_Tag("Normal");
    pSubStateMachine->Get_State("SwitchInAttack")->Set_Tag("Attack");
    pSubStateMachine->Get_State("SwitchInParryAid")->Set_Tag("ParryAid");

    return pInstance;
}

void CMiyabiState_SwitchIn::Enter(CMiyabi* pOwner)
{
    pOwner->Push_Invincible();

    switch (pOwner->Get_Switch())
    {
    case CCharacter::SWITCH::NORMAL:
        m_pSubStateMachine->Set_DefaultState("SwitchInNormal");
        break;
    case CCharacter::SWITCH::ATTACK:
        m_pSubStateMachine->Set_DefaultState("SwitchInAttack");
        break;
    case CCharacter::SWITCH::PARRYAID:
        m_pSubStateMachine->Set_DefaultState("SwitchInParryAid");
        break;
    }

    pOwner->Get_StateMachine()->Reset_Trigger("ToIdle");
    pOwner->Get_StateMachine()->Reset_Trigger("ResetState");
    pOwner->Reset_InputInfo();

    m_pSubStateMachine->Reset_Trigger("Complete");
    m_pSubStateMachine->Set_Int("ExitMode", 0);

    __super::Enter(pOwner);
}

void CMiyabiState_SwitchIn::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    auto pMiyabiState = pOwner->Get_StateMachine();
    if (pMiyabiState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
            Is_AnimEnd())
        {
            pMiyabiState->Set_Trigger("SwitchOut");
            pMiyabiState->Set_Bool("OutReserve", false);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");

        _int iExitMode = m_pSubStateMachine->Get_Int("ExitMode");
        m_pSubStateMachine->Set_Int("ExitMode", 0);

        CStateMachine<CMiyabi>* pRootFSM = pOwner->Get_StateMachine();

        switch (iExitMode)
        {
        case 1:
            pRootFSM->Set_Trigger("ToMove");
            pRootFSM->Set_Int("MoveEntryMode", 2);
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

void CMiyabiState_SwitchIn::Exit(CMiyabi* pOwner)
{
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

_bool CMiyabiState_SwitchIn::Handle_Transition(CMiyabi* pOwner, const string& strState)
{
    if (m_pSubStateMachine->Get_CurrentStateName() == "SwitchInParryAid")
    {
        IHState<CMiyabi>* pState = dynamic_cast<IHState<CMiyabi>*>(m_pSubStateMachine->Get_CurrentState());
        if (pState->Get_SubStateMachine()->Get_CurrentState()->Get_Tag() != "End")
            return false;
    }
    else if (m_pSubStateMachine->Get_CurrentStateName() == "SwitchInAttack")
    {
        IHState<CMiyabi>* pState = dynamic_cast<IHState<CMiyabi>*>(m_pSubStateMachine->Get_CurrentState());
        if (pState->Get_SubStateMachine()->Get_CurrentState()->Get_Tag() != "End")
            return false;
    }
    return true;
}
