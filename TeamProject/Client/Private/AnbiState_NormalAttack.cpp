#include "pch.h"
#include "AnbiState_NormalAttack.h"
#include "Anbi.h"
#include "GameInstance.h"
#include "Animator3D.h"

void CAnbiState_NormalAttack::Enter(CAnbi* pOwner)
{
    m_iComboIndex = 0;

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CAnbi>::Create();

        m_pSubStateMachine->Register_State("Attack_01", CAnbiState_Attack_01::Create());
        m_pSubStateMachine->Register_State("Attack_02", CAnbiState_Attack_02::Create());
        m_pSubStateMachine->Register_State("Attack_03", CAnbiState_Attack_03::Create());
        m_pSubStateMachine->Register_State("Attack_04", CAnbiState_Attack_04::Create());
        m_pSubStateMachine->Register_State("Attack_End", CAnbiState_Attack_End::Create());

        m_pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");


        // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
        vector<CStateMachine<CAnbi>::CONDITION_INFO> comboConditions;
        comboConditions.push_back({ CStateMachine<CAnbi>::CONDITION_TRIGGER, "NextCombo", 0.f });
        comboConditions.push_back({ CStateMachine<CAnbi>::CONDITION_ANIMATION_END, "", 0.f });

        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);


        // End 전이
        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
            CStateMachine<CAnbi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
            CStateMachine<CAnbi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
            CStateMachine<CAnbi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
            CStateMachine<CAnbi>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    // 트리거 초기화
    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CAnbiState_NormalAttack::Update(CAnbi* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    __super::Update(pOwner, dt);

    if (m_pSubStateMachine)
    {
        string strCurrent = m_pSubStateMachine->Get_CurrentStateName();
        IBaseState<CAnbi>* pCurrent = m_pSubStateMachine->Get_CurrentState();

        _float currentSubProgress = pCurrent ? pCurrent->Get_AnimProgress() : -1.f;

        OutputDebugStringA(("NormalAttack: CurrentSub=" + strCurrent +
            ", SubProgress=" + to_string(currentSubProgress) +
            ", MyProgress=" + to_string(m_fAnimProgress) + "\n").c_str());

        if (strCurrent == "Attack_End")
        {
            if (pCurrent && pCurrent->Is_AnimEnd())
                m_fAnimProgress = 1.f;
        }
    }
}

void CAnbiState_NormalAttack::Exit(CAnbi* pOwner)
{
}


#pragma region SubStates
void CAnbiState_Attack_01::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Attack_Normal_01")
        .Apply();
}

void CAnbiState_Attack_01::Exit(CAnbi* pOwner)
{
    static_cast<CAnbiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(0);
}

void CAnbiState_Attack_02::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Attack_Normal_02")
        .Apply();
}

void CAnbiState_Attack_02::Exit(CAnbi* pOwner)
{
    static_cast<CAnbiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CAnbiState_Attack_03::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Attack_Normal_03")
        .Apply();
}

void CAnbiState_Attack_03::Exit(CAnbi* pOwner)
{
    static_cast<CAnbiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(2);
}

void CAnbiState_Attack_04::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Attack_Normal_04")
        .Apply();
}

void CAnbiState_Attack_04::Exit(CAnbi* pOwner)
{
    static_cast<CAnbiState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CAnbiState_Attack_End::Enter(CAnbi* pOwner)
{
    CAnbiState_NormalAttack* pParent = static_cast<CAnbiState_NormalAttack*>(m_pParentState);
    _uint iIndex = pParent ? pParent->Get_ComboIndex() : 0;

    const string arrEndAnims[5] =
    {
        "Avatar_Female_Size02_Anbi_Ani_Attack_Normal_01_End",
        "Avatar_Female_Size02_Anbi_Ani_Attack_Normal_02_End",
        "Avatar_Female_Size02_Anbi_Ani_Attack_Normal_03_End",
        "Avatar_Female_Size02_Anbi_Ani_Attack_Normal_04_End",
        "Avatar_Female_Size02_Anbi_Ani_Attack_Normal_04_Perfect_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Apply();
}
void CAnbiState_Attack_End::Update(CAnbi* pOwner, _float dt)
{
    if (pOwner->Is_Input())
    {
        m_fAnimProgress = 1.f;
    }
}
#pragma endregion