#include "pch.h"
#include "CorinState_NormalAttack.h"
#include "Corin.h"
#include "GameInstance.h"
#include "Animator3D.h"

void CCorinState_NormalAttack::Enter(CCorin* pOwner)
{
    pOwner->Lock_Move();
    m_iComboIndex = 0;

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();

        m_pSubStateMachine->Register_State("Attack_01", CCorinState_Attack_01::Create());
        m_pSubStateMachine->Register_State("Attack_02", CCorinState_Attack_02::Create());
        m_pSubStateMachine->Register_State("Attack_03", CCorinState_Attack_03::Create());
        m_pSubStateMachine->Register_State("Attack_04", CCorinState_Attack_04::Create());
        m_pSubStateMachine->Register_State("Attack_05", CCorinState_Attack_05::Create());
        m_pSubStateMachine->Register_State("Attack_End", CCorinState_Attack_End::Create());

        m_pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");


        // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
        vector<CStateMachine<CCorin>::CONDITION_INFO> comboConditions;
        comboConditions.push_back({ CStateMachine<CCorin>::CONDITION_TRIGGER, "NextCombo", 0.f });
        comboConditions.push_back({ CStateMachine<CCorin>::CONDITION_ANIMATION_END, "", 0.f });

        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions);


        // End 전이
        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    // 트리거 초기화
    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CCorinState_NormalAttack::Update(CCorin* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    __super::Update(pOwner, dt);
}

void CCorinState_NormalAttack::Exit(CCorin* pOwner)
{
}


#pragma region SubStates
void CCorinState_Attack_01::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_01")
        .Speed(1.2f)
        .ReserveSpeed(0.37f, 0.45f, 0.4f, EaseType::OutQuart)
        .ReserveSpeed(0.45f, 0.5f, 1.2f, EaseType::Linear)
        .Apply();
}

void CCorinState_Attack_01::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.37f)
    {
        pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::COUNT, DAMAGE_TYPE::NORMAL, 1.f, 0.f,4 });
    }
    if (m_fAnimProgress >= 0.45f)
    {
        pOwner->End_AttackCollider("Saw");
    }
}

void CCorinState_Attack_01::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(0);
}

void CCorinState_Attack_02::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_02")
        .Speed(1.5f)
        .ReserveSpeed(0.25f, 0.35f, 0.4f, EaseType::OutQuart)
        .ReserveSpeed(0.35f, 0.40f, 1.5f, EaseType::Linear)
        .Apply();
}

void CCorinState_Attack_02::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.25f)
    {
        pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::COUNT, DAMAGE_TYPE::NORMAL, 1.f,0.f, 4 });
    }
    if (m_fAnimProgress >= 0.35f)
    {
        pOwner->End_AttackCollider("Saw");
    }
}

void CCorinState_Attack_02::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CCorinState_Attack_03::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_03")
        .Speed(1.5f)
        .ReserveSpeed(0.5f, 1.f, 0.55f, EaseType::OutQuart)
        .Apply();
}

void CCorinState_Attack_03::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.5f)
    {
        pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::COUNT, DAMAGE_TYPE::NORMAL, 1.f, 0.15f, 7 });
    }
    if (m_fAnimProgress >= 0.99f)
    {
        pOwner->End_AttackCollider("Saw");
    }
}

void CCorinState_Attack_03::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(2);
}

void CCorinState_Attack_04::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_04")
        .Speed(1.f)
        .ReserveSpeed(0.22f, 0.46f, 0.55f, EaseType::OutQuart)
        .ReserveSpeed(0.46f, 0.51f, 1.f, EaseType::Linear)
        .Apply();
}

void CCorinState_Attack_04::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.22f)
    {
        pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::COUNT, DAMAGE_TYPE::NORMAL, 1.f, 0.f, 4 });
    }
    if (m_fAnimProgress >= 0.46f)
    {
        pOwner->End_AttackCollider("Saw");
    }
}

void CCorinState_Attack_04::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CCorinState_Attack_05::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_05")
        .Speed(1.5f)
        .ReserveSpeed(0.4f, 1.f, 0.5f, EaseType::OutQuart)
        .Apply();
}

void CCorinState_Attack_05::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
    if (m_fAnimProgress >= 0.4f)
    {
        pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::COUNT, DAMAGE_TYPE::NORMAL, 1.f, 0.15f, 7 });
    }
    if (m_fAnimProgress >= 0.99f)
    {
        pOwner->End_AttackCollider("Saw");
    }
}

void CCorinState_Attack_05::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(4);
}

void CCorinState_Attack_End::Enter(CCorin* pOwner)
{
    CCorinState_NormalAttack* pParent = static_cast<CCorinState_NormalAttack*>(m_pParentState);
    _uint iIndex = pParent ? pParent->Get_ComboIndex() : 0;

    const string arrEndAnims[5] =
    {
        pOwner->Get_Name() + "Attack_Normal_01_End",
        pOwner->Get_Name() + "Attack_Normal_02_End",
        pOwner->Get_Name() + "Attack_Normal_03_End",
        pOwner->Get_Name() + "Attack_Normal_04_End",
        pOwner->Get_Name() + "Attack_Normal_05_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Speed(1.2f)
        .Apply();

    pOwner->Unlock_Move();
}
#pragma endregion


