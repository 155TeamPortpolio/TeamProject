#include "pch.h"
#include "SacrificeState_Walk_Phase1.h"
#include "Sacrifice.h"
#include "Helper_Func.h"

void CSacrificeState_Walk_Phase1::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Walk_F_Phase1", CSacrificeState_Walk_F_Phase1::Create());
		m_pSubStateMachine->Register_State("Walk_B_Phase1", CSacrificeState_Walk_B_Phase1::Create());
		m_pSubStateMachine->Register_State("Walk_R_Phase1", CSacrificeState_Walk_R_Phase1::Create());
		m_pSubStateMachine->Register_State("Walk_L_Phase1", CSacrificeState_Walk_L_Phase1::Create());

		__super::Enter(pOwner);
	}

	_uint iRandIndex = Helper::Get_Random_Int(0, 3);

	if (0 == iRandIndex)
		m_pSubStateMachine->Change_State("Walk_F_Phase1");
	else if (1 == iRandIndex)
		m_pSubStateMachine->Change_State("Walk_B_Phase1");
	else if (2 == iRandIndex)
		m_pSubStateMachine->Change_State("Walk_R_Phase1");
	else
		m_pSubStateMachine->Change_State("Walk_L_Phase1");
}

void CSacrificeState_Walk_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Walk_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Walk_F_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Walk_F").Loop(true).Apply();
}

void CSacrificeState_Walk_F_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Walk_F_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Walk_B_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Walk_B").Loop(true).Apply();
}

void CSacrificeState_Walk_B_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Walk_B_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Walk_R_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Walk_R").Loop(true).Apply();
}

void CSacrificeState_Walk_R_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Walk_R_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Walk_L_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Walk_L").Loop(true).Apply();
}

void CSacrificeState_Walk_L_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Walk_L_Phase1::Exit(CSacrifice* pOwner)
{
}
