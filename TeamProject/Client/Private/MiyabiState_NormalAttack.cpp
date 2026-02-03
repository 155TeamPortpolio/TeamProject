#include "pch.h"
#include "MiyabiState_NormalAttack.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "BattlePlayer.h"
#include "Miyabi.h"

#include "Animator3D.h"
#include "CharacterController.h"

CMiyabiState_NormalAttack* CMiyabiState_NormalAttack::Create()
{
    auto pInstance = new CMiyabiState_NormalAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Attack_01", CMiyabiState_Attack_01::Create());
    pSubStateMachine->Register_State("Attack_02", CMiyabiState_Attack_02::Create());
    pSubStateMachine->Register_State("Attack_03", CMiyabiState_Attack_03::Create());
    pSubStateMachine->Register_State("Attack_04", CMiyabiState_Attack_04::Create());
    pSubStateMachine->Register_State("Attack_05", CMiyabiState_Attack_05::Create());
    pSubStateMachine->Register_State("Attack_06", CMiyabiState_Attack_06::Create());
    pSubStateMachine->Register_State("Attack_End", CMiyabiState_Attack_End::Create());

    pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");


    // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
    vector<CStateMachine<CMiyabi>::CONDITION_INFO> comboConditions;
    comboConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_TRIGGER, "NextCombo", 0.f });
    comboConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_ANIMATION_GREATER, "", 0.7f });

    pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
    pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
    pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);
    pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions);
    pSubStateMachine->Register_Transition("Attack_05", "Attack_06", comboConditions);

    // End 전이
    pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_05", "Attack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_06", "Attack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("Attack_01");

    return pInstance;
}

void CMiyabiState_NormalAttack::Enter(CMiyabi* pOwner)
{
    pOwner->Lock_Move();
    m_iComboIndex = 0;
    // 트리거 초기화
    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CMiyabiState_NormalAttack::Update(CMiyabi* pOwner, _float dt)
{
    if (InputDevice()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    auto pMiyabiState = pOwner->Get_StateMachine();
    if (pMiyabiState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "Attack_End" ||
            Is_AnimEnd())
        {
            pMiyabiState->Set_Trigger("SwitchOut");
            pMiyabiState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CMiyabiState_NormalAttack::Exit(CMiyabi* pOwner)
{
    pOwner->Reset_ReserveCombo();
    __super::Exit(pOwner);
}


#pragma region SubStates
void CMiyabiState_Attack_01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_01")
        .Apply();
}

void CMiyabiState_Attack_01::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Attack_01::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(0);
}

void CMiyabiState_Attack_02::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_02")
        .Apply();
}

void CMiyabiState_Attack_02::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Attack_02::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CMiyabiState_Attack_03::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_03")
        .Apply();
}

void CMiyabiState_Attack_03::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Attack_03::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(2);
}

void CMiyabiState_Attack_04::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_04_02")
        .Apply();
}

void CMiyabiState_Attack_04::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Attack_04::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CMiyabiState_Attack_05::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_05")
        .Apply();
}

void CMiyabiState_Attack_05::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Attack_05::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(4);
}

void CMiyabiState_Attack_06::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_06")
        .Apply();
}

void CMiyabiState_Attack_06::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Attack_06::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(5);
}

void CMiyabiState_Attack_End::Enter(CMiyabi* pOwner)
{
    CMiyabiState_NormalAttack* pParent = static_cast<CMiyabiState_NormalAttack*>(m_pParentState);
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
        .Apply();
    pOwner->Unlock_Move();
}

#pragma endregion