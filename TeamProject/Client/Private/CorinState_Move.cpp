#include "pch.h"
#include "CorinState_Move.h"
#include "CorinState_Walk.h"
#include "CorinState_Run.h"

#include "Corin.h"

#include "CharacterController.h"

CCorinState_Move* CCorinState_Move::Create()
{
    auto pInstance = new CCorinState_Move();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Walk", CCorinState_Walk::Create());
    pSubStateMachine->Register_State("Run", CCorinState_Run::Create());

    pSubStateMachine->Register_Transition("Walk", "Run",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToRun");
    pSubStateMachine->Register_Transition("Run", "Walk",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToWalk");

    pSubStateMachine->Set_DefaultState("Walk");

    return pInstance;
}

void CCorinState_Move::Enter(CCorin* pOwner)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("MoveEntryMode");
    pOwner->Get_StateMachine()->Set_Int("MoveEntryMode", 0);

    switch (iEntryMode)
    {
    case 2: // Run Loop
        m_pSubStateMachine->Set_DefaultState("Run");
        m_pSubStateMachine->Set_Int("RunEntryMode", 0);
        break;
    case 1: //  Run End
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

void CCorinState_Move::Exit(CCorin* pOwner)
{
    __super::Exit(pOwner);
}
