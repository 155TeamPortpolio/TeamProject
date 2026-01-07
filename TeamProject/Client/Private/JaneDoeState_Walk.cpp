#include "pch.h"
#include "JaneDoeState_Walk.h"

#include "JaneDoe.h"
#include "JaneDoeState_Move.h"

#include "CharacterController.h"



void CJaneDoeState_Walk::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Start", CJaneDoeState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CJaneDoeState_Walk_Loop::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_Walk_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CJaneDoeState_Walk::Update(CJaneDoe* pOwner, _float dt)
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

void CJaneDoeState_Walk_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Walk_Start")
        .Loop(false)
        .Speed(1.2f)
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
        .Speed(1.2f)
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
