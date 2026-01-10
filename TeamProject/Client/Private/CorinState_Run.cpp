#include "pch.h"
#include "CorinState_Run.h"
#include "Corin.h"

#include "CorinState_Move.h"
#include "CharacterController.h"

void CCorinState_Run::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Loop", CCorinState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_Run_End::Create());
        m_pSubStateMachine->Register_State("Turnback", CCorinState_Run_Turnback::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");
        // 반대 방향 입력
        m_pSubStateMachine->Register_Transition("Loop", "Turnback",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToTurnback");
        // Turnback -> End: 입력 없음
        m_pSubStateMachine->Register_Transition("Turnback", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");
        // Turnback -> Loop: 애니메이션 끝 + 입력 유지
        m_pSubStateMachine->Register_Transition("Turnback", "Loop",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToLoop");

        m_pSubStateMachine->Set_DefaultState("Loop");
    }

    IHState<CCorin>* pMoveState = Get_ParentState();
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

void CCorinState_Run::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
}

void CCorinState_Run_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run")
        .Loop(true)
        .Apply();
}

void CCorinState_Run_Loop::Update(CCorin* pOwner, _float dt)
{
    if (pOwner->Is_OppositeInput())
    {
        IHState<CCorin>* pRunState = Get_ParentState();
        if (pRunState && pRunState->Get_SubStateMachine())
        {
            pRunState->Get_SubStateMachine()->Set_Trigger("ToTurnback");
            return;
        }
    }
    pOwner->Process_RootMotion(dt);
}

void CCorinState_Run_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run_End")
        .Apply();
}

void CCorinState_Run_Turnback::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "TurnBack")
        .Speed(1.2f)
        .Apply();
}

void CCorinState_Run_Turnback::Update(CCorin* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);
    }

    if (m_fAnimProgress > 0.5f && pOwner->Is_Move())
    {
        IHState<CCorin>* pRunState = Get_ParentState();
        if (pRunState && pRunState->Get_SubStateMachine())
            pRunState->Get_SubStateMachine()->Set_Trigger("ToLoop");
    }
}
