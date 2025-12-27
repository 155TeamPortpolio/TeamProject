#include "pch.h"
#include "SacrificeState_Idle.h"
#include "Sacrifice.h"

void CSacrificeState_Idle::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Idle_Phase1", CSacrificeState_Idle_Phase1::Create());
		m_pSubStateMachine->Register_State("Idle_Phase2", CSacrificeState_Idle_Phase2::Create());

		m_pSubStateMachine->Set_DefaultState("Idle_Phase1");
	}

	__super::Enter(pOwner);
}

void CSacrificeState_Idle::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner,dt);

	if (m_pSubStateMachine)
	{

	}
}

void CSacrificeState_Idle::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase1::Enter(CSacrifice* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("SacrificeBringer_Ani_P1_Idle");
}

void CSacrificeState_Idle_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Idle_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase2::Enter(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Idle_Phase2::Exit(CSacrifice* pOwner)
{
}
