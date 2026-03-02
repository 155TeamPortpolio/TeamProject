#include "pch.h"
#include "JaneDoeState_Walk.h"

#include "JaneDoe.h"
#include "JaneDoeState_Move.h"

#include "CharacterController.h"

CJaneDoeState_Walk* CJaneDoeState_Walk::Create()
{
    auto pInstance = new CJaneDoeState_Walk();
    pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Start", CJaneDoeState_Walk_Start::Create());
    pSubStateMachine->Register_State("Loop", CJaneDoeState_Walk_Loop::Create());
    pSubStateMachine->Register_State("End", CJaneDoeState_Walk_End::Create());

    pSubStateMachine->Get_State("End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Start", "Loop",
        CStateMachine<CJaneDoe>::CONDITION_ANIMATION_GREATER, "", 0.93);

    pSubStateMachine->Register_Transition("Start", "End",
        CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "IsMove");

    pSubStateMachine->Register_Transition("Loop", "End",
        CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "IsMove");

    pSubStateMachine->Set_DefaultState("Start");

    return pInstance;
}

void CJaneDoeState_Walk::Enter(CJaneDoe* pOwner)
{
    __super::Enter(pOwner);
}

void CJaneDoeState_Walk::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
    if (m_pSubStateMachine->Get_CurrentStateName() == "Loop")
    {
        auto pLoop = m_pSubStateMachine->Get_CurrentState();
        if (pLoop && pLoop->Get_AnimProgress()>= 0.93)
        {
            auto pMoveState = Get_ParentState();
            if (pMoveState && pMoveState->Get_SubStateMachine())
                pMoveState->Get_SubStateMachine()->Set_Trigger("ToRun");
        }
    }
}

void CJaneDoeState_Walk::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}

void CJaneDoeState_Walk_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk_Start")
        .Loop(false)
        .Apply();
}

void CJaneDoeState_Walk_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CJaneDoeState_Walk_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk")
        .Loop(false)
        .Apply();
}

void CJaneDoeState_Walk_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CJaneDoeState_Walk_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk_End")
        .Apply();
}

void CJaneDoeState_Walk_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt, ENUM(CCharacter::ROOTMOTION_MASK::MOVE) |
        ENUM(CCharacter::ROOTMOTION_MASK::QUATERNION));
}
