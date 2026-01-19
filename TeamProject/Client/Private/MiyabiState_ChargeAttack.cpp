#include "pch.h"
#include "MiyabiState_ChargeAttack.h"
#include "MiyabiState_Attack.h"
#include "Miyabi.h"
#include "GameInstance.h"
#include "Animator3D.h"

// ===================== ChargeAttack =====================

void CMiyabiState_ChargeAttack::Enter(CMiyabi* pOwner)
{
    m_iChargeLevel = 0;

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();

        m_pSubStateMachine->Register_State("Charge_Start", CMiyabiState_Charge_Start::Create());
        m_pSubStateMachine->Register_State("Charge_Start_02", CMiyabiState_Charge_Start_02::Create());
        m_pSubStateMachine->Register_State("Charge_Start_03", CMiyabiState_Charge_Start_03::Create());
        m_pSubStateMachine->Register_State("Charge_Attack01", CMiyabiState_Charge_Attack01::Create());
        m_pSubStateMachine->Register_State("Charge_Attack03", CMiyabiState_Charge_Attack03::Create());
        m_pSubStateMachine->Register_State("Charge_End", CMiyabiState_Charge_End::Create());

        m_pSubStateMachine->Get_State("Charge_End")->Set_Tag("End");

        // 충전 단계 전이: AnimEnd로 다음 단계
        m_pSubStateMachine->Register_Transition("Charge_Start", "Charge_Start_02",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Charge_Start_02", "Charge_Start_03",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

        // 3단 충전 완료 시 자동 공격
        m_pSubStateMachine->Register_Transition("Charge_Start_03", "Charge_Attack03",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

        // 버튼 놓으면 공격 (Release 트리거)
        m_pSubStateMachine->Register_Transition("Charge_Start", "Charge_Attack01",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Release");
        m_pSubStateMachine->Register_Transition("Charge_Start_02", "Charge_Attack01",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Release");
        m_pSubStateMachine->Register_Transition("Charge_Start_03", "Charge_Attack03",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Release");

        // 공격 → End
        m_pSubStateMachine->Register_Transition("Charge_Attack01", "Charge_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Charge_Attack03", "Charge_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Charge_Start");
    }
    m_pSubStateMachine->Reset_Trigger("Release");
    __super::Enter(pOwner);
}

void CMiyabiState_ChargeAttack::Update(CMiyabi* pOwner, _float dt)
{
    // 버튼 놓으면 Release 트리거
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Away(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("Release");

    __super::Update(pOwner, dt);

    // Charge_End 종료 시 ChargeAttack 종료
    if (m_pSubStateMachine)
    {
        IBaseState<CMiyabi>* pCurrent = m_pSubStateMachine->Get_CurrentState();
        string strCurrent = m_pSubStateMachine->Get_CurrentStateName();

        if (strCurrent == "Charge_End" && pCurrent && pCurrent->Is_AnimEnd())
            m_fAnimProgress = 1.f;
        else
            m_fAnimProgress = 0.f;
    }
}

void CMiyabiState_ChargeAttack::Exit(CMiyabi* pOwner)
{
}

// ===================== Sub States =====================

// Charge_Start (1단)
void CMiyabiState_Charge_Start::Enter(CMiyabi* pOwner)
{
    CMiyabiState_ChargeAttack* pChargeAttack = static_cast<CMiyabiState_ChargeAttack*>(m_pParentState);
    CMiyabiState_Attack* pAttack = pChargeAttack ? static_cast<CMiyabiState_Attack*>(pChargeAttack->Get_ParentState()) : nullptr;

    _bool bFromCombo = false;
    if (pAttack && pAttack->Get_SubStateMachine())
        bFromCombo = (pAttack->Get_SubStateMachine()->Get_PrevStateName() == "NormalAttack");

    if (bFromCombo)
        pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Start_Front")
        .Apply();
    else
        pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Start")
        .Apply();
}
void CMiyabiState_Charge_Start::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Charge_Start::Exit(CMiyabi* pOwner)
{
    CMiyabiState_ChargeAttack* pParent = static_cast<CMiyabiState_ChargeAttack*>(m_pParentState);
    if (pParent)
        pParent->Set_ChargeLevel(1);
}

// Charge_Start_02 (2단)
void CMiyabiState_Charge_Start_02::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Start_02")
        .Apply();
}

void CMiyabiState_Charge_Start_02::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Charge_Start_02::Exit(CMiyabi* pOwner)
{
    CMiyabiState_ChargeAttack* pParent = static_cast<CMiyabiState_ChargeAttack*>(m_pParentState);
    if (pParent)
        pParent->Set_ChargeLevel(2);
}

// Charge_Start_03 (3단)
void CMiyabiState_Charge_Start_03::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Start_03")
        .Apply();
}

void CMiyabiState_Charge_Start_03::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Charge_Start_03::Exit(CMiyabi* pOwner)
{
    CMiyabiState_ChargeAttack* pParent = static_cast<CMiyabiState_ChargeAttack*>(m_pParentState);
    if (pParent)
        pParent->Set_ChargeLevel(3);
}

// Charge_Attack01 (1~2단 공격)
void CMiyabiState_Charge_Attack01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Attack01")
        .Apply();
}

// Charge_Attack03 (3단 공격)
void CMiyabiState_Charge_Attack03::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Attack03")
        .Apply();
}

// Charge_End
void CMiyabiState_Charge_End::Enter(CMiyabi* pOwner)
{
    CMiyabiState_ChargeAttack* pParent = static_cast<CMiyabiState_ChargeAttack*>(m_pParentState);
    _uint iLevel = pParent ? pParent->Get_ChargeLevel() : 1;

    if (iLevel >= 3)
        pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Attack03_End")
        .Apply();
    else
        pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_ChargeAttack_Attack01_End")
        .Apply();
}

void CMiyabiState_Charge_End::Update(CMiyabi* pOwner, _float dt)
{
    if (pOwner->Is_Input())
    {
        m_fAnimProgress = 1.f;
    }
}

void CMiyabiState_Charge_End::Exit(CMiyabi* pOwner)
{

}
