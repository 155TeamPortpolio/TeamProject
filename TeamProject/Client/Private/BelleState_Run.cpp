#include "pch.h"
#include "BelleState_Run.h"
#include "Belle.h"

void CBelleState_Run::Enter(CBelle* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CBelle>::Create();
        m_pSubStateMachine->Register_State("Loop", CBelleState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CBelleState_Run_End::Create());
        m_pSubStateMachine->Register_State("Turnback", CBelleState_Run_Turnback::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CBelle>::CONDITION_BOOL_FALSE, "IsMove");
        // 반대 방향 입력
        m_pSubStateMachine->Register_Transition("Loop", "Turnback",
            CStateMachine<CBelle>::CONDITION_TRIGGER, "ToTurnback");
        // Turnback -> End: 입력 없음
        m_pSubStateMachine->Register_Transition("Turnback", "End",
            CStateMachine<CBelle>::CONDITION_BOOL_FALSE, "IsMove");
        // Turnback -> Loop: 애니메이션 끝 + 입력 유지
        m_pSubStateMachine->Register_Transition("Turnback", "Loop",
            CStateMachine<CBelle>::CONDITION_TRIGGER, "ToLoop");

        m_pSubStateMachine->Set_DefaultState("Loop");
    }

    IHState<CBelle>* pMoveState = Get_ParentState();
    _int iRunEntryMode = 0;
    if (pMoveState && pMoveState->Get_SubStateMachine())
    {
        iRunEntryMode = pMoveState->Get_SubStateMachine()->Get_Int("RunEntryMode");
        pMoveState->Get_SubStateMachine()->Set_Int("RunEntryMode", 0);
    }

    if (iRunEntryMode == 1)
        m_pSubStateMachine->Set_DefaultState("End");
    else
        m_pSubStateMachine->Set_DefaultState("Loop");

    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
    __super::Enter(pOwner);
}

void CBelleState_Run::Update(CBelle* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
}

void CBelleState_Run_Loop::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "MainCity_Run_Loop")
        .Loop(true)
        .EndAt(0.93)
        .Apply();
}

void CBelleState_Run_Loop::Update(CBelle* pOwner, _float dt)
{
   if (pOwner->Is_OppositeInput())
   {
       IHState<CBelle>* pRunState = Get_ParentState();
       if (pRunState && pRunState->Get_SubStateMachine())
       {
           pRunState->Get_SubStateMachine()->Set_Trigger("ToTurnback");
           return;
       }
   }
    pOwner->Process_RootMotion(dt);
}

void CBelleState_Run_Turnback::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "MainCity_Run_TurnBack")
        .Loop(true)
        .Apply();
}

void CBelleState_Run_Turnback::Update(CBelle* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CBelleState_Run_End::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "MainCity_Run_End_L")
        .Loop(true)
        .Apply();
}
