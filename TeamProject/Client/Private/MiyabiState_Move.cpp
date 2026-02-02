#include "pch.h"
#include "MiyabiState_Move.h"
#include "MiyabiState_Walk.h"
#include "MiyabiState_Run.h"
#include "Miyabi.h"

#include "CharacterController.h"

CMiyabiState_Move* CMiyabiState_Move::Create()
{
    auto pInstance = new CMiyabiState_Move();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Walk", CMiyabiState_Walk::Create());
    pSubStateMachine->Register_State("Run", CMiyabiState_Run::Create());

    pSubStateMachine->Register_Transition("Walk", "Run",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToRun");
    pSubStateMachine->Register_Transition("Run", "Walk",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToWalk");

    pSubStateMachine->Set_DefaultState("Walk");

    return pInstance;
}

void CMiyabiState_Move::Enter(CMiyabi* pOwner)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("MoveEntryMode");
    pOwner->Get_StateMachine()->Set_Int("MoveEntryMode", 0);

    switch (iEntryMode)
    {
    // Run
    case 1:
        m_pSubStateMachine->Set_DefaultState("Run");
        m_pSubStateMachine->Set_Int("RunEntryMode", 1);
        break;
    case 2:
        m_pSubStateMachine->Set_DefaultState("Run");
        m_pSubStateMachine->Set_Int("RunEntryMode", 0);
        break;
    default:
        m_pSubStateMachine->Set_DefaultState("Walk");
        break;
    }

    __super::Enter(pOwner);
}

void CMiyabiState_Move::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CMiyabiState_Move::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}