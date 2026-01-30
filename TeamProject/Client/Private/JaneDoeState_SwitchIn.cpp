#include "pch.h"
#include "JaneDoeState_SwitchIn.h"

#include "Character.h"
#include "JaneDoe.h"
#include "JaneDoeState_SwitchInAttack.h"
#include "JaneDoeState_SwitchInParryAid.h"
#include "JaneDoeState_SwitchInNormal.h"

CJaneDoeState_SwitchIn* CJaneDoeState_SwitchIn::Create()
{
    auto pInstance = new CJaneDoeState_SwitchIn();
    pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInNormal", CJaneDoeState_SwitchInNormal::Create());
    pSubStateMachine->Register_State("SwitchInAttack", CJaneDoeState_SwitchInAttack::Create());
    pSubStateMachine->Register_State("SwitchInParryAid", CJaneDoeState_SwitchInParryAid::Create());

    pSubStateMachine->Get_State("SwitchInNormal")->Set_Tag("Normal");
    pSubStateMachine->Get_State("SwitchInAttack")->Set_Tag("Attack");
    pSubStateMachine->Get_State("SwitchInParryAid")->Set_Tag("ParryAid");

    return pInstance;
}

void CJaneDoeState_SwitchIn::Enter(CJaneDoe* pOwner)
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

void CJaneDoeState_SwitchIn::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    auto pJaneDoeState = pOwner->Get_StateMachine();
    if (pJaneDoeState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
            Is_AnimEnd())
        {
            pJaneDoeState->Set_Trigger("SwitchOut");
            pJaneDoeState->Set_Bool("OutReserve", false);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");

        _int iExitMode = m_pSubStateMachine->Get_Int("ExitMode");
        m_pSubStateMachine->Set_Int("ExitMode", 0);

        CStateMachine<CJaneDoe>* pRootFSM = pOwner->Get_StateMachine();

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

void CJaneDoeState_SwitchIn::Exit(CJaneDoe* pOwner)
{
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

_bool CJaneDoeState_SwitchIn::Handle_Transition(CJaneDoe* pOwner, const string& strState)
{
    if (m_pSubStateMachine->Get_CurrentStateName() == "SwitchInParryAid")
    {
        IHState<CJaneDoe>* pState = dynamic_cast<IHState<CJaneDoe>*>(m_pSubStateMachine->Get_CurrentState());
        if (pState->Get_SubStateMachine()->Get_CurrentState()->Get_Tag() != "End")
            return false;
    }
    else if (m_pSubStateMachine->Get_CurrentStateName() == "SwitchInAttack")
    {
        IHState<CJaneDoe>* pState = dynamic_cast<IHState<CJaneDoe>*>(m_pSubStateMachine->Get_CurrentState());
        if (pState->Get_SubStateMachine()->Get_CurrentState()->Get_Tag() != "End")
            return false;
    }
    return true;
}
