#include "pch.h"
#include "JaneDoeState_Move.h"
#include "JaneDoeState_Walk.h"
#include "JaneDoeState_Run.h"

#include "JaneDoe.h"

#include "CharacterController.h"

void CJaneDoeState_Move::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Walk", CJaneDoeState_Walk::Create());
        m_pSubStateMachine->Register_State("Run", CJaneDoeState_Run::Create());

        m_pSubStateMachine->Register_Transition("Walk", "Run",
            CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToRun");
        m_pSubStateMachine->Register_Transition("Run", "Walk",
            CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToWalk");

        m_pSubStateMachine->Set_DefaultState("Walk");
    }

    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("MoveEntryMode");
    pOwner->Get_StateMachine()->Set_Int("MoveEntryMode", 0);

    switch (iEntryMode)
    {
    case 2:
        m_pSubStateMachine->Set_DefaultState("Run");
        m_pSubStateMachine->Set_Int("RunEntryMode", 0);
        break;
    case 1:
        m_pSubStateMachine->Set_DefaultState("Run");
        m_pSubStateMachine->Set_Int("RunEntryMode", 1);
        break;
    default:
        m_pSubStateMachine->Set_DefaultState("Walk");
        break;
    }

    __super::Enter(pOwner);
}

void CJaneDoeState_Move::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CJaneDoeState_Move::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}
