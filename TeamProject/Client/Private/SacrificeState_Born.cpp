#include "pch.h"
#include "SacrificeState_Born.h"
#include "StateMachine.h"
#include "Sacrifice.h"

void CSacrificeState_Born::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Born_Phase1", CSacrificeState_Born_Phase1::Create());
		//m_pSubStateMachine->Register_State("Idle_Phase2", CSacrificeState_Idle_Phase2::Create());

		m_pSubStateMachine->Set_DefaultState("Born_Phase1");
	}

	__super::Enter(pOwner);
}

void CSacrificeState_Born::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Born::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Born_Phase1::Enter(CSacrifice* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Set_Animation("SacrificeBringer_Ani_P1_Born")
		.Loop(false)
		.Apply();
}

void CSacrificeState_Born_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Born_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Born_Phase2::Enter(CSacrifice* pOwner)
{
}

void CSacrificeState_Born_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Born_Phase2::Exit(CSacrifice* pOwner)
{
}
