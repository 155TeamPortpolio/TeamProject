#include "pch.h"
#include "BelleState_Run.h"
#include "Belle.h"

void CBelleState_Run::Enter(CBelle* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBelle>::Create();
        m_pSubStateMachine->Register_State("Start", CBelleState_Run_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CBelleState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CBelleState_Run_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CBelle>::CONDITION_ANIMATION_GREATER, "", 0.97);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CBelle>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CBelle>::CONDITION_BOOL_FALSE, "IsMove");


        m_pSubStateMachine->Set_DefaultState("Start");
    }

    m_pSubStateMachine->Set_DefaultState("Start");

    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
    __super::Enter(pOwner);
}

void CBelleState_Run::Update(CBelle* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
}

void CBelleState_Run::Exit(CBelle* pOwner)
{
    __super::Exit(pOwner);
}

void CBelleState_Run_Start::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Ani_MainCity_Run_Start")
        .Loop(false)
        .Apply();
    static_cast<CBelleState_Run*>(m_pParentState)->Set_LastFoot("R");
}

void CBelleState_Run_Start::Update(CBelle* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "L" || Event.Tag == "R")
            static_cast<CBelleState_Run*>(m_pParentState)->Set_LastFoot(Event.Tag);
    }
}

void CBelleState_Run_Loop::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Ani_MainCity_Run_Loop")
        .Loop(true)
        .EndAt(0.953)
        .Apply();
}

void CBelleState_Run_Loop::Update(CBelle* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "L" || Event.Tag == "R")
            static_cast<CBelleState_Run*>(m_pParentState)->Set_LastFoot(Event.Tag);
    }
}

void CBelleState_Run_End::Enter(CBelle* pOwner)
{
    string strWalkEnd = "Ani_MainCity_Run_End_" + static_cast<CBelleState_Run*>(m_pParentState)->Get_LastFoot();
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + strWalkEnd)
        .Loop(false)
        .Apply();
}

void CBelleState_Run_End::Update(CBelle* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    if (m_fAnimProgress >= 0.23f)
    {
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
    }
}
