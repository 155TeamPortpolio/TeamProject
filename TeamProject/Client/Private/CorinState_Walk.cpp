#include "pch.h"
#include "CorinState_Walk.h"

#include "Corin.h"
#include "CorinState_Move.h"

#include "CharacterController.h"



void CCorinState_Walk::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Start", CCorinState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CCorinState_Walk_Loop::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_Walk_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CCorinState_Walk::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
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

void CCorinState_Walk_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Walk_Start")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_Walk_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_Walk_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Walk")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_Walk_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_Walk_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run_Start_End")
        .Apply();
}
