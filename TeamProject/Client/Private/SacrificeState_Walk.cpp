#include "pch.h"
#include "SacrificeState_Walk.h"
#include "Sacrifice.h"

void CSacrificeState_Walk::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Walk_F", CSacrificeState_Walk_F::Create());
		//m_pSubStateMachine->Register_State("Idle_Phase2", CSacrificeState_Idle_Phase2::Create());

		m_pSubStateMachine->Set_DefaultState("Walk_F");
	}
}

void CSacrificeState_Walk::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Walk::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Walk_F::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Walk_F")
		.Loop(true)
		.Apply();
}

void CSacrificeState_Walk_F::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Walk_F::Exit(CSacrifice* pOwner)
{
}
