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

        // 추가 입력 시 다음 콤보
        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_05",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_06",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    __super::Enter(pOwner);
}

void CMiyabiState_Attack::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    // 현재 공격 애니메이션 끝나고 추가 입력 없으면 Attack 상태 종료
    if (m_pSubStateMachine)
    {
        IBaseState<CMiyabi>* pCurrent = m_pSubStateMachine->Get_CurrentState();
        if (pCurrent && pCurrent->Is_AnimEnd())
            m_fAnimProgress = 1.f;
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
}
