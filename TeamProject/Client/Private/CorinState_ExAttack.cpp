#include "pch.h"
#include "GameInstance.h"
#include "CorinState_ExAttack.h"
#include "Corin.h"

void CCorinState_ExAttack::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();

        m_pSubStateMachine->Register_State("Start", CCorinState_ExAttack_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CCorinState_ExAttack_Loop::Create());
        m_pSubStateMachine->Register_State("Loop_Walk", CCorinState_ExAttack_Loop_Walk::Create());
        m_pSubStateMachine->Register_State("Explode", CCorinState_ExAttack_Explode::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_ExAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Loop", "Loop_Walk",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToWalk");
        m_pSubStateMachine->Register_AnyStateTransition("Explode",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToExplode");
        m_pSubStateMachine->Register_Transition("Explode", "End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    auto tDesc = pOwner->Get_EnergyDesc();
    _bool bEnhanced = tDesc.fCurrentEnergy >= tDesc.fSpecialEnergy;
    m_pSubStateMachine->Set_Bool("Enhanced", bEnhanced);
    m_pSubStateMachine->Set_Bool("ExFinished", false);

    __super::Enter(pOwner);
}

void CCorinState_ExAttack::Update(CCorin* pOwner, _float dt)
{
    if (!m_pSubStateMachine->Get_Bool("ExFinished") && !InputDevice()->Key_Down('E'))
    {
        if (m_pSubStateMachine->Get_Bool("Enhanced"))
        {
            m_pSubStateMachine->Set_Int("ExplodeEntryMode", 2);
        }
        else
            m_pSubStateMachine->Set_Int("ExplodeEntryMode", 1);
        m_pSubStateMachine->Set_Trigger("ToExplode");
    }
    __super::Update(pOwner, dt);
}

void CCorinState_ExAttack_Start::Enter(CCorin* pOwner)
{
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();
    if (Get_ParentState()->Get_SubStateMachine()->Get_Bool("Enhanced"))
    {
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02")
            .Speed(1.5f)
            .Apply();
    }
    else
    {
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01")
            .Speed(1.f)
            .Apply();
    }
}

void CCorinState_ExAttack_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_ExAttack_Loop::Enter(CCorin* pOwner)
{
    if (Get_ParentState()->Get_SubStateMachine()->Get_Bool("Enhanced"))
        pOwner->Unlock_Move();
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Loop")
        .Loop(true)
        .Speed(1.f)
        .Apply();
}

void CCorinState_ExAttack_Loop::Update(CCorin* pOwner, _float dt)
{
    _bool bEnhanced = Get_ParentState()->Get_SubStateMachine()->Get_Bool("Enhanced");
    if (pOwner->Is_Move_Buffer() && bEnhanced)
    {
        Get_ParentState()->Get_SubStateMachine()->Set_Trigger("ToWalk");
    }

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_ExAttack_Loop_Walk::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Walk_Loop")
        .Loop(true)
        .Apply();
}

void CCorinState_ExAttack_Loop_Walk::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
}

void CCorinState_ExAttack_Loop_Walk::Exit(CCorin* pOwner)
{
    pOwner->Lock_Move();
}

void CCorinState_ExAttack_Explode::Enter(CCorin* pOwner)
{
    Get_ParentState()->Get_SubStateMachine()->Set_Bool("ExFinished", true);
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();

    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    _int iEntryMode = pSubStateMachine->Get_Int("ExplodeEntryMode");
    pSubStateMachine->Set_Int("ExplodeEntryMode", 0);

    switch (iEntryMode)
    {
    case 1:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01_Explode")
            .Apply();
        break;
    case 2:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02_Explode")
            .Apply();
        break;
    }
    pSubStateMachine->Set_Int("EndEntryMode", iEntryMode);
}

void CCorinState_ExAttack_Explode::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_ExAttack_End::Enter(CCorin* pOwner)
{
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();

    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    _int iEntryMode = pSubStateMachine->Get_Int("EndEntryMode");
    pSubStateMachine->Set_Int("EndEntryMode", 0);

    switch (iEntryMode)
    {
    case 1:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01_End")
            .Apply();
        break;
    case 2:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02_End")
            .Apply();
        break;
    }
}