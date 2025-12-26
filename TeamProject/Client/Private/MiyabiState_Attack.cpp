#include "pch.h"
#include "MiyabiState_Attack.h"
#include "Miyabi.h"

void CMiyabiState_Attack::Enter(CMiyabi* pOwner)
{
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

        // 애니메이션 끝나면 End로
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

        // ExitTime + Trigger 조건으로 콤보 연결
        vector<CStateMachine<CMiyabi>::CONDITION_INFO> comboConditions;
        comboConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack", 0.f });

        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions, true, 0.5f);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions, true, 0.5f);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions, true, 0.5f);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions, true, 0.5f);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_06", comboConditions, true, 0.5f);

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    __super::Enter(pOwner);
}

void CMiyabiState_Attack::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    // Attack_End 애니메이션 끝나면 Attack 상태 종료
    if (m_pSubStateMachine)
    {
        string strCurrent = m_pSubStateMachine->Get_CurrentStateName();
        IBaseState<CMiyabi>* pCurrent = m_pSubStateMachine->Get_CurrentState();

        if (strCurrent == "Attack_End" && pCurrent && pCurrent->Is_AnimEnd())
        {
            m_fAnimProgress = 1.f;
        }
        else
        {
            m_fAnimProgress = 0.f;
        }
    }
}

void CMiyabiState_Attack::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Attack_01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_01");
}

void CMiyabiState_Attack_01::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_01::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_Attack*>(m_pParentState)->Set_AttackIndex(1);
}

void CMiyabiState_Attack_02::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_02");
}

void CMiyabiState_Attack_02::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_02::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_Attack*>(m_pParentState)->Set_AttackIndex(2);
}

void CMiyabiState_Attack_03::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_03");
}

void CMiyabiState_Attack_03::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_03::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_Attack*>(m_pParentState)->Set_AttackIndex(3);
}

void CMiyabiState_Attack_04::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_04_02");
}

void CMiyabiState_Attack_04::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_04::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_Attack*>(m_pParentState)->Set_AttackIndex(4);
}

void CMiyabiState_Attack_05::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_05");
}

void CMiyabiState_Attack_05::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_05::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_Attack*>(m_pParentState)->Set_AttackIndex(5);
}

void CMiyabiState_Attack_06::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Attack_06");
}

void CMiyabiState_Attack_06::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_06::Exit(CMiyabi* pOwner)
{
    static_cast<CMiyabiState_Attack*>(m_pParentState)->Set_AttackIndex(6);
}

void CMiyabiState_Attack_End::Enter(CMiyabi* pOwner)
{
    CMiyabiState_Attack* pParent = static_cast<CMiyabiState_Attack*>(m_pParentState);

    _uint iIndex = pParent ? pParent->Get_AttackIndex() : 1;

    string strAnim = "Avatar_Female_Size02_Unagi_Ani_Attack_0" + to_string(iIndex) + "_End";

    if (iIndex == 4)
        strAnim = "Avatar_Female_Size02_Unagi_Ani_Attack_04_02_End";

    pOwner->Get_Animator()->Set_Animation(strAnim);
}

void CMiyabiState_Attack_End::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Attack_End::Exit(CMiyabi* pOwner)
{
}
