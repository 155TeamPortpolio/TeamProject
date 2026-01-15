#include "pch.h"
#include "BelleState_Walk.h"
#include "Belle.h"

void CBelleState_Walk::Enter(CBelle* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBelle>::Create();
        m_pSubStateMachine->Register_State("Start", CBelleState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CBelleState_Walk_Loop::Create());
        m_pSubStateMachine->Register_State("End", CBelleState_Walk_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CBelle>::CONDITION_ANIMATION_GREATER, "", 0.93);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CBelle>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CBelle>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CBelleState_Walk::Update(CBelle* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
    if (m_pSubStateMachine->Get_CurrentStateName() == "Loop")
    {
        auto pLoop = m_pSubStateMachine->Get_CurrentState();
        if (pLoop && pLoop->Get_AnimProgress() >= 0.93)
        {
            auto pMoveState = Get_ParentState();
            if (pMoveState && pMoveState->Get_SubStateMachine())
                pMoveState->Get_SubStateMachine()->Set_Trigger("ToRun");
        }
    }
}

void CBelleState_Walk_Start::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Suibianguan_Ani_MainCity_Walk_Start")
        .Loop(false)
        .Apply();
}

void CBelleState_Walk_Start::Update(CBelle* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CBelleState_Walk_Loop::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Suibianguan_Ani_MainCity_Walk_Loop")
        .Loop(true)
        .Apply();
}

void CBelleState_Walk_Loop::Update(CBelle* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CBelleState_Walk_End::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Suibianguan_Ani_MainCity_Walk_End_L")
        .Loop(false)
        .Apply();
}
