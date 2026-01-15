#include "pch.h"
#include "Belle.h"
#include "BelleState_Move.h"

#include "BelleState_Run.h"
#include "BelleState_Walk.h"

void CBelleState_Move::Enter(CBelle* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBelle>::Create();
        m_pSubStateMachine->Register_State("Walk", CBelleState_Walk::Create());
        m_pSubStateMachine->Register_State("Run", CBelleState_Run::Create());

        m_pSubStateMachine->Register_Transition("Walk", "Run",
            CStateMachine<CBelle>::CONDITION_TRIGGER, "ToRun");
        m_pSubStateMachine->Register_Transition("Run", "Walk",
            CStateMachine<CBelle>::CONDITION_TRIGGER, "ToWalk");

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

void CBelleState_Move::Update(CBelle* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}
