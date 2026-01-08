#include "pch.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoe.h"
#include "GameInstance.h"
#include "Animator3D.h"

void CJaneDoeState_NormalAttack::Enter(CJaneDoe* pOwner)
{
    m_iComboIndex = 0;

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        m_pSubStateMachine->Register_State("Attack_01", CJaneDoeState_Attack_01::Create());
        m_pSubStateMachine->Register_State("Attack_02", CJaneDoeState_Attack_02::Create());
        m_pSubStateMachine->Register_State("Attack_03", CJaneDoeState_Attack_03::Create());
        m_pSubStateMachine->Register_State("Attack_04", CJaneDoeState_Attack_04::Create());
        m_pSubStateMachine->Register_State("Attack_05", CJaneDoeState_Attack_05::Create());
        m_pSubStateMachine->Register_State("Attack_06", CJaneDoeState_Attack_06::Create());
        m_pSubStateMachine->Register_State("Attack_End", CJaneDoeState_Attack_End::Create());

        m_pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");


        // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
        vector<CStateMachine<CJaneDoe>::CONDITION_INFO> comboConditions;
        comboConditions.push_back({ CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "NextCombo", 0.f });
        comboConditions.push_back({ CStateMachine<CJaneDoe>::CONDITION_TIME_GREATER, "", 0.6f });

        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_06", comboConditions);


        // End 전이
        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_06", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    // 트리거 초기화
    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CJaneDoeState_NormalAttack::Update(CJaneDoe* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    __super::Update(pOwner, dt);
}

void CJaneDoeState_NormalAttack::Exit(CJaneDoe* pOwner)
{
}


void CJaneDoeState_Attack_01::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_01")
        .Speed(2.f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_01::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Attack_01::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(0);
}

void CJaneDoeState_Attack_02::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_02")
        .Speed(2.f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_02::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Attack_02::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CJaneDoeState_Attack_03::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_03")
        .Speed(1.5f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_03::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Attack_03::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(2);
}

void CJaneDoeState_Attack_04::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_04")
        .Speed(2.f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_04::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Attack_04::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CJaneDoeState_Attack_05::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_05")
        .Speed(1.5f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_05::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Attack_05::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(4);
}


void CJaneDoeState_Attack_06::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_06")
        .Speed(1.5f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_06::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Attack_06::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(5);
}

void CJaneDoeState_Attack_End::Enter(CJaneDoe* pOwner)
{
    CJaneDoeState_NormalAttack* pParent = static_cast<CJaneDoeState_NormalAttack*>(m_pParentState);
    _uint iIndex = pParent ? pParent->Get_ComboIndex() : 0;

    const string arrEndAnims[6] =
    {
        pOwner->Get_Name() + "Attack_Normal_01_End",
        pOwner->Get_Name() + "Attack_Normal_02_End",
        pOwner->Get_Name() + "Attack_Normal_03_End",
        pOwner->Get_Name() + "Attack_Normal_04_End",
        pOwner->Get_Name() + "Attack_Normal_05_End",
        pOwner->Get_Name() + "Attack_Normal_06_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Speed(1.2f)
        .Apply();
}