#include "pch.h"
#include "CorinState_Move.h"
#include "CorinState_Walk.h"
#include "CorinState_Run.h"

#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Move::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Walk", CCorinState_Walk::Create());
        m_pSubStateMachine->Register_State("Run", CCorinState_Run::Create());
        m_pSubStateMachine->Register_Transition("Walk", "Run",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToRun");
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

void CCorinState_Move::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}
