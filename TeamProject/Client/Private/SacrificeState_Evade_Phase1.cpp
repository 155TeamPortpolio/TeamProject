#include "pch.h"
#include "SacrificeState_Evade_Phase1.h"
#include "Sacrifice.h"
#include "Helper_Func.h"

void CSacrificeState_Evade_Phase1::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();

		__super::Enter(pOwner);
	}

	_uint iRandIndex = Helper::Get_Random_Int(0, 2);
	if (0 == iRandIndex)
		m_pSubStateMachine->Change_State("Evade_B_Phase1");
	else if(1 == iRandIndex)
		m_pSubStateMachine->Change_State("Evade_R_Phase1");
	else
		m_pSubStateMachine->Change_State("Evade_L_Phase1");
}

void CSacrificeState_Evade_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Evade_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Evade_Phase1::Register_States()
{
	m_pSubStateMachine->Register_State("Evade_B_Phase1", CSacrificeState_Evade_B_Phase1::Create());
	m_pSubStateMachine->Register_State("Evade_R_Phase1", CSacrificeState_Evade_R_Phase1::Create());
	m_pSubStateMachine->Register_State("Evade_L_Phase1", CSacrificeState_Evade_L_Phase1::Create());
}

void CSacrificeState_Evade_B_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Evade_B").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Evade_B_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Evade_B_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Evade_R_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Evade_R").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Evade_R_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Evade_R_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Evade_L_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Evade_L").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Evade_L_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Evade_L_Phase1::Exit(CSacrifice* pOwner)
{
}
