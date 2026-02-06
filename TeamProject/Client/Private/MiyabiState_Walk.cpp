#include "pch.h"
#include "MiyabiState_Walk.h"
#include "Miyabi.h"
#include "MiyabiState_Move.h"

#include "CharacterController.h"

CMiyabiState_Walk* CMiyabiState_Walk::Create()
{
    auto pInstance = new CMiyabiState_Walk();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Start", CMiyabiState_Walk_Start::Create());
    pSubStateMachine->Register_State("Loop", CMiyabiState_Walk_Loop::Create());
    pSubStateMachine->Register_State("End", CMiyabiState_Walk_End::Create());

    pSubStateMachine->Get_State("End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Start", "Loop",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Register_Transition("Start", "End",
        CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

    pSubStateMachine->Register_Transition("Loop", "End",
        CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

    pSubStateMachine->Set_DefaultState("Start");

    return pInstance;
}

void CMiyabiState_Walk::Enter(CMiyabi* pOwner)
{
    __super::Enter(pOwner);
}

void CMiyabiState_Walk::Update(CMiyabi* pOwner, _float dt)
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

void CMiyabiState_Walk::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}

void CMiyabiState_Walk_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk_Start")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_Walk_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CMiyabiState_Walk_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_Walk_Loop::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CMiyabiState_Walk_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run_Start_End")
        .Apply();
}

void CMiyabiState_Walk_End::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt, ENUM(CCharacter::ROOTMOTION_MASK::MOVE) |
        ENUM(CCharacter::ROOTMOTION_MASK::QUATERNION));
}