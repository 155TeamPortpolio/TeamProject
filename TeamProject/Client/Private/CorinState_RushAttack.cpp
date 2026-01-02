#include "pch.h"
#include "CorinState_RushAttack.h"
#include "Corin.h"

void CCorinState_RushAttack::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Rush_Start", CCorinState_Rush_Start::Create());
        m_pSubStateMachine->Register_State("Rush_Explode", CCorinState_Rush_Explode::Create());
        m_pSubStateMachine->Register_State("Rush_End", CCorinState_Rush_End::Create());
        
        m_pSubStateMachine->Get_State("Rush_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Rush_Start", "Rush_Explode",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush_Explode", "Rush_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Rush_Start");
    }
    __super::Enter(pOwner);
}

void CCorinState_RushAttack::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CCorinState_Rush_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Attack_Rush")
        .Speed(1.f)
        .Apply();
}

void CCorinState_Rush_Explode::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Attack_Rush_Explode")
        .Speed(2.f)
        .Apply();
}

void CCorinState_Rush_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Attack_Rush_End")
        .Speed(2.f)
        .Apply();
}
