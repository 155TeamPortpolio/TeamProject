#include "pch.h"
#include "CorinState_Walk.h"

#include "Corin.h"
#include "CorinState_Move.h"

#include "CharacterController.h"

CCorinState_Walk* CCorinState_Walk::Create()
{
    auto pInstance = new CCorinState_Walk();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Start", CCorinState_Walk_Start::Create());
    pSubStateMachine->Register_State("Loop", CCorinState_Walk_Loop::Create());
    pSubStateMachine->Register_State("End", CCorinState_Walk_End::Create());

    pSubStateMachine->Get_State("End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Start", "Loop",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Register_Transition("Start", "End",
        CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

    pSubStateMachine->Register_Transition("Loop", "End",
        CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

    pSubStateMachine->Set_DefaultState("Start");

    return pInstance;
}

void CCorinState_Walk::Enter(CCorin* pOwner)
{
    __super::Enter(pOwner);
}

void CCorinState_Walk::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
    if (m_pSubStateMachine->Get_CurrentStateName() == "Loop")
    {
        auto pLoop = m_pSubStateMachine->Get_CurrentState();
        if (pLoop && pLoop->Is_AnimEnd())
        {
            auto pMoveState = Get_ParentState();
            if (pMoveState && pMoveState->Get_SubStateMachine())
                pMoveState->Get_SubStateMachine()->Set_Trigger("ToRun");
        }
    }
}

void CCorinState_Walk::Exit(CCorin* pOwner)
{
    __super::Exit(pOwner);
}

void CCorinState_Walk_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk_Start")
        .Loop(false)
        .EndAt(0.93f)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_Walk_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_Walk_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk")
        .Loop(false)
        .EndAt(0.93f)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_Walk_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_Walk_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run_Start_End")
        .Apply();
}

void CCorinState_Walk_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt, ENUM(CCharacter::ROOTMOTION_MASK::MOVE) |
        ENUM(CCharacter::ROOTMOTION_MASK::QUATERNION));
}
