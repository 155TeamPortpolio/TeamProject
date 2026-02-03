#include "pch.h"
#include "SacrificeState_Parry_Phase1.h"
#include "Sacrifice.h"
#include "Helper_Func.h"

void CSacrificeState_Parry_Phase1::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Parry_R_Phase1",CSacrificeState_Parry_R_Phase1::Create());
		m_pSubStateMachine->Register_State("Parry_L_Phase1", CSacrificeState_Parry_L_Phase1::Create());

		__super::Enter(pOwner);
	}

	switch (pOwner->Get_AttackSide())
	{
	case CEnemy::ATTACK_SIDE::LEFT:
	{
		m_pSubStateMachine->Change_State("Parry_L_Phase1");
	}break;
	case CEnemy::ATTACK_SIDE::RIGHT:
	{
		m_pSubStateMachine->Change_State("Parry_R_Phase1");
	}break;
	default:
		break;
	}
}

void CSacrificeState_Parry_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Parry_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Parry_R_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Hit_Parry_Right").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Parry_R_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Parry_R_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Parry_L_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Hit_Parry_Left").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Parry_L_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Parry_L_Phase1::Exit(CSacrifice* pOwner)
{
}
