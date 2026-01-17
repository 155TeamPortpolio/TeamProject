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
    // 강화 상태 판정
    auto tDesc = pOwner->Get_EnergyDesc();
    _bool bEnhanced = tDesc.fCurrentEnergy >= tDesc.fSpecialEnergy;
    m_pSubStateMachine->Set_Bool("Enhanced", bEnhanced);
    // current, special 변경
    if (bEnhanced)
    {
        pOwner->Set_CurrentEnergy(tDesc.fCurrentEnergy - 20.f);
        pOwner->Set_SpecialEnergy(20.f);
    }
    //초기화
    m_pSubStateMachine->Set_Bool("ExFinished", false);

    __super::Enter(pOwner);
}

void CCorinState_ExAttack::Update(CCorin* pOwner, _float dt)
{
    if (!m_pSubStateMachine->Get_Bool("ExFinished"))
    {
        if (!InputDevice()->Key_Down('E'))
        {
            if (m_pSubStateMachine->Get_Bool("Enhanced"))
                m_pSubStateMachine->Set_Int("ExplodeEntryMode", 2);
            else
                m_pSubStateMachine->Set_Int("ExplodeEntryMode", 1);
            m_pSubStateMachine->Set_Trigger("ToExplode");
        }
        else if (m_pSubStateMachine->Get_Bool("Enhanced"))
        {
            if (pOwner->Get_EnergyDesc().fCurrentEnergy <= pOwner->Get_EnergyDesc().fSpecialEnergy)
            {
                m_pSubStateMachine->Set_Int("ExplodeEntryMode", 2);
                m_pSubStateMachine->Set_Trigger("ToExplode");
            }
        }
    }
    __super::Update(pOwner, dt);
}

void CCorinState_ExAttack::Exit(CCorin* pOwner)
{
    __super::Exit(pOwner);
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

    if (m_fAnimProgress >= 0.4f)
        pOwner->Begin_AttackCollider("Saw", {HIT_TYPE::ONCE, DAMAGE_TYPE::NORMAL, 1.f});
    if (m_fAnimProgress >= 0.5f)
        pOwner->End_AttackCollider("Saw");
    if (m_fAnimProgress >= 0.55f)
        pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::INTERVAL, DAMAGE_TYPE::NORMAL, 1.f, 0.01f, 1 });
}

void CCorinState_ExAttack_Loop::Enter(CCorin* pOwner)
{
    if (Get_ParentState()->Get_SubStateMachine()->Get_Bool("Enhanced"))
    {
        pOwner->Unlock_Move();
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Loop")
            .Loop(true)
            .Speed(1.f)
            .Apply();
    }
    else
    {
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Loop")
            .Loop(false)
            .Speed(1.f)
            .Apply();
    }
}

void CCorinState_ExAttack_Loop::Update(CCorin* pOwner, _float dt)
{
    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    _bool bEnhanced = pSubStateMachine->Get_Bool("Enhanced");
    if (pOwner->Is_Move_Buffer() && bEnhanced)
    {
        pSubStateMachine->Set_Trigger("ToWalk");
    }
    if (!bEnhanced && Is_AnimEnd())
    {
        pSubStateMachine->Set_Int("ExplodeEntryMode", 1);
        pSubStateMachine->Set_Trigger("ToExplode");
    }

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    auto desc = pOwner->Get_EnergyDesc();
    pOwner->Set_CurrentEnergy(desc.fCurrentEnergy - desc.fEnergyWeight * dt);
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

    auto desc = pOwner->Get_EnergyDesc();
    pOwner->Set_CurrentEnergy(desc.fCurrentEnergy - desc.fEnergyWeight * dt);
}

void CCorinState_ExAttack_Loop_Walk::Exit(CCorin* pOwner)
{
    pOwner->Lock_Move();
}

void CCorinState_ExAttack_Explode::Enter(CCorin* pOwner)
{
    pOwner->End_AttackCollider("Saw");
    pOwner->Set_SpecialEnergy(80.f);
    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    pSubStateMachine->Set_Bool("ExFinished", true);
    // 강화 상태일때 20소모
    if (pSubStateMachine->Get_Bool("Enhanced"))
    {
        pOwner->Set_CurrentEnergy(pOwner->Get_EnergyDesc().fCurrentEnergy - 20.f);
    }
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();

    _int iEntryMode = pSubStateMachine->Get_Int("ExplodeEntryMode");
    pSubStateMachine->Set_Int("ExplodeEntryMode", 0);

    switch (iEntryMode)
    {
    case 1:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01_Explode")
            .Loop(false)
            .Apply();
        break;
    case 2:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02_Explode")
            .Loop(false)
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
            .Speed(1.5f)
            .Apply();
        break;
    case 2:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02_End")
            .Speed(1.5f)
            .Apply();
        break;
    }
}