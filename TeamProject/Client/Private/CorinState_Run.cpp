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

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

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

    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());

    __super::Enter(pOwner);
}

void CCorinState_Run::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
}

void CCorinState_Run_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run")
        .Loop(true)
        .Apply();
}

void CCorinState_Run_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_Run_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run_End")
        .Apply();
}

void CCorinState_Run_End::Update(CCorin* pOwner, _float dt)
{
    if (pOwner->Is_Input())
    {
        m_fAnimProgress = 1.f;
    }
}