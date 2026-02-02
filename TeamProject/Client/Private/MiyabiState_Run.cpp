#include "pch.h"
#include "MiyabiState_Run.h"
#include "Miyabi.h"

#include "MiyabiState_Move.h"
#include "CharacterController.h"

CMiyabiState_Run* CMiyabiState_Run::Create()
{
    auto pInstance = new CMiyabiState_Run();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Loop", CMiyabiState_Run_Loop::Create());
    pSubStateMachine->Register_State("End", CMiyabiState_Run_End::Create());
    pSubStateMachine->Register_State("Turnback", CMiyabiState_Run_Turnback::Create());

    pSubStateMachine->Get_State("End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Loop", "End",
        CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");
    // 반대 방향 입력
    pSubStateMachine->Register_Transition("Loop", "Turnback",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToTurnback");
    // Turnback -> End: 입력 없음
    pSubStateMachine->Register_Transition("Turnback", "End",
        CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");
    // Turnback -> Loop: 애니메이션 끝 + 입력 유지
    pSubStateMachine->Register_Transition("Turnback", "Loop",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToLoop");

    pSubStateMachine->Set_DefaultState("Loop");

    return pInstance;
}

void CMiyabiState_Run::Enter(CMiyabi* pOwner)
{
    IHState<CMiyabi>* pMoveState = Get_ParentState();
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

void CMiyabiState_Run::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move_Buffer());
}

void CMiyabiState_Run::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}

void CMiyabiState_Run_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run")
        .Loop(true)
        .EndAt(0.93f)
        .Apply();

    IHState<CMiyabi>* pRunState = Get_ParentState();
    if (pRunState && pRunState->Get_SubStateMachine())
    {
        m_fTurnbackCooldown = pRunState->Get_SubStateMachine()->Get_Float("TurnbackCooldown");
        pRunState->Get_SubStateMachine()->Set_Float("TurnbackCooldown", 0.f);
    }
}

void CMiyabiState_Run_Loop::Update(CMiyabi* pOwner, _float dt)
{
    if (m_fTurnbackCooldown > 0.f)
    {
        m_fTurnbackCooldown -= dt;
        if (pOwner->Is_OppositeInput())
            pOwner->Set_ResetMove(true);
    }
    else if (pOwner->Is_OppositeInput())
    {
        IHState<CMiyabi>* pRunState = Get_ParentState();
        if (pRunState && pRunState->Get_SubStateMachine())
        {
            pRunState->Get_SubStateMachine()->Set_Trigger("ToTurnback");
            return;
        }
    }
    pOwner->Process_RootMotion(dt);
}


void CMiyabiState_Run_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Run_End")
        .Apply();
}

void CMiyabiState_Run_End::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt, ENUM(CCharacter::ROOTMOTION_MASK::MOVE) |
        ENUM(CCharacter::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Run_Turnback::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "TurnBack")
        .Speed(1.f)
        .EndAt(0.35f)
        .Apply();
    pOwner->Reset_InputBuffer();
    pOwner->Set_ResetMove(true);
}

void CMiyabiState_Run_Turnback::Update(CMiyabi* pOwner, _float dt)
{
    CCharacter::ROOTMOTION_DESC desc;
    desc.fMoveWeight = 1.f;
    desc.iModeMask = ENUM(CCharacter::ROOTMOTION_MASK::MOVE) |
        ENUM(CCharacter::ROOTMOTION_MASK::QUATERNION);
    pOwner->Process_RootMotion(dt, desc);

    if (Is_AnimEnd() && pOwner->Is_Move())
    {
        IHState<CMiyabi>* pRunState = Get_ParentState();
        if (pRunState && pRunState->Get_SubStateMachine())
        {
            pRunState->Get_SubStateMachine()->Set_Float("TurnbackCooldown", 0.3f);
            pRunState->Get_SubStateMachine()->Set_Trigger("ToLoop");
        }
    }
}
