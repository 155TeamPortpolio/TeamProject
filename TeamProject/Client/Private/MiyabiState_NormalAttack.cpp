#include "pch.h"
#include "MiyabiState_NormalAttack.h"
#include "Miyabi.h"
#include "GameInstance.h"
#include "Animator3D.h"

void CMiyabiState_NormalAttack::Enter(CMiyabi* pOwner)
{
    m_iComboIndex = 0;

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();

        m_pSubStateMachine->Register_State("Attack_01", CMiyabiState_Attack_01::Create());
        m_pSubStateMachine->Register_State("Attack_02", CMiyabiState_Attack_02::Create());
        m_pSubStateMachine->Register_State("Attack_03", CMiyabiState_Attack_03::Create());
        m_pSubStateMachine->Register_State("Attack_04", CMiyabiState_Attack_04::Create());
        m_pSubStateMachine->Register_State("Attack_05", CMiyabiState_Attack_05::Create());
        m_pSubStateMachine->Register_State("Attack_06", CMiyabiState_Attack_06::Create());
        m_pSubStateMachine->Register_State("Attack_End", CMiyabiState_Attack_End::Create());

        m_pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");


        // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
        vector<CStateMachine<CMiyabi>::CONDITION_INFO> comboConditions;
        comboConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_TRIGGER, "NextCombo", 0.f });
        comboConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_ANIMATION_END, "", 0.f });

        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_06", comboConditions);

        // End 전이
        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_06", "Attack_End",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    // 트리거 초기화
    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CMiyabiState_NormalAttack::Update(CMiyabi* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    __super::Update(pOwner, dt);

    if (m_pSubStateMachine)
    {
        string strCurrent = m_pSubStateMachine->Get_CurrentStateName();
        IBaseState<CMiyabi>* pCurrent = m_pSubStateMachine->Get_CurrentState();

        if (strCurrent == "Attack_End")
        {
            if (pCurrent && pCurrent->Is_AnimEnd())
                m_fAnimProgress = 1.f;
        }
    }
}

void CMiyabiState_NormalAttack::Exit(CMiyabi* pOwner)
{
}


#pragma region SubStates
void CMiyabiState_Attack_01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_01")
        .Apply();
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

void CMiyabiState_Attack_02::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CMiyabiState_Attack_03::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_03")
        .Apply();
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

void CMiyabiState_Attack_04::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CMiyabiState_Attack_05::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_05")
        .Apply();
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
        "Avatar_Female_Size02_Unagi_Ani_Attack_01_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_02_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_03_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_04_02_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_05_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_06_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Apply();
}
void CMiyabiState_Attack_End::Update(CMiyabi* pOwner, _float dt)
{
    if (pOwner->Is_Input())
    {
        m_fAnimProgress = 1.f;
    }
}
#pragma endregion