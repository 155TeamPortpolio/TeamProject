#include "pch.h"
#include "SacrificeState_Evade_Phase2.h"
#include "Sacrifice.h"
#include "Helper_Func.h"

void CSacrificeState_Evade_Phase2::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();

		__super::Enter(pOwner);
	}

	_uint iRandIndex = Helper::Get_Random_Int(0, 2);
	if (0 == iRandIndex)
		m_pSubStateMachine->Change_State("Evade_B_Phase2");
	else if (1 == iRandIndex)
		m_pSubStateMachine->Change_State("Evade_R_Phase2");
	else
		m_pSubStateMachine->Change_State("Evade_L_Phase2");
}

void CSacrificeState_Evade_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Evade_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Evade_Phase2::Register_States()
{
	m_pSubStateMachine->Register_State("Evade_B_Phase2", CSacrificeState_Evade_B_Phase2::Create());
	m_pSubStateMachine->Register_State("Evade_R_Phase2", CSacrificeState_Evade_R_Phase2::Create());
	m_pSubStateMachine->Register_State("Evade_L_Phase2", CSacrificeState_Evade_L_Phase2::Create());
}

void CSacrificeState_Evade_B_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Evade_B").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Evade_B_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Evade_B_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Evade_R_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Evade_R").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Evade_R_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Evade_R_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Evade_L_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Evade_L").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Evade_L_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Evade_L_Phase2::Exit(CSacrifice* pOwner)
{
}