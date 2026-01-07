#include "pch.h"
#include "JaneDoeState_Run.h"
#include "JaneDoe.h"

#include "JaneDoeState_Move.h"
#include "CharacterController.h"

void CJaneDoeState_Run::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Loop", CJaneDoeState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_Run_End::Create());
        m_pSubStateMachine->Register_State("Turnback", CJaneDoeState_Run_Turnback::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "IsMove");
        // 반대 방향 입력
        m_pSubStateMachine->Register_Transition("Loop", "Turnback",
            CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToTurnback");
        // Turnback -> End: 입력 없음
        m_pSubStateMachine->Register_Transition("Turnback", "End",
            CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "IsMove");
        // Turnback -> Loop: 애니메이션 끝 + 입력 유지
        m_pSubStateMachine->Register_Transition("Turnback", "Loop",
            CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToLoop");

        m_pSubStateMachine->Set_DefaultState("Loop");
    }

    IHState<CJaneDoe>* pMoveState = Get_ParentState();
    _int iRunEntryMode = 0;
    if (pMoveState && pMoveState->Get_SubStateMachine())
    {
        iRunEntryMode = pMoveState->Get_SubStateMachine()->Get_Int("RunEntryMode");
        pMoveState->Get_SubStateMachine()->Set_Int("RunEntryMode", 0);
    }

    pOwner->Reset_LastValidKey();

    if (iRunEntryMode == 1)
        m_pSubStateMachine->Set_DefaultState("End");
    else
        m_pSubStateMachine->Set_DefaultState("Loop");

    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
    __super::Enter(pOwner);
}

void CJaneDoeState_Run::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
}

void CJaneDoeState_Run_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run")
        .Loop(true)
        .Apply();
}

void CJaneDoeState_Run_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    if (pOwner->Is_OppositeInput())
    {
        IHState<CJaneDoe>* pRunState = Get_ParentState();
        if (pRunState && pRunState->Get_SubStateMachine())
        {
            pRunState->Get_SubStateMachine()->Set_Trigger("ToTurnback");
            return;
        }
    }
    pOwner->Process_RootMotion(dt);
}

void CJaneDoeState_Run_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run_End")
        .Apply();
}

void CJaneDoeState_Run_Turnback::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "TurnBack")
        .Speed(1.2f)
        .Apply();

    pOwner->Reset_LastValidKey();
}

void CJaneDoeState_Run_Turnback::Update(CJaneDoe* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);
    }

    if (m_fAnimProgress > 0.5f && pOwner->Is_Move())
    {
        IHState<CJaneDoe>* pRunState = Get_ParentState();
        if (pRunState && pRunState->Get_SubStateMachine())
            pRunState->Get_SubStateMachine()->Set_Trigger("ToLoop");
    }
}
