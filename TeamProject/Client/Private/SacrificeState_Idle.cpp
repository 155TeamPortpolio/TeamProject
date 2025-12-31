#include "pch.h"
#include "SacrificeState_Idle.h"
#include "Sacrifice.h"

void CSacrificeState_Idle::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Idle_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Idle_Phase2::Create());

		__super::Enter(pOwner);
	}

	CSacrifice::PHASE currPhse = pOwner->GetCurrPhase();
	switch (currPhse)
	{
	case CSacrifice::PHASE::PHASE1:
	{
		m_pSubStateMachine->Change_State("Phase1");
	}break;
	case CSacrifice::PHASE::PHASE2:
	{
		m_pSubStateMachine->Change_State("Phase2");
	}break;
	default:
		break;
	}
}

void CSacrificeState_Idle::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner,dt);
}

void CSacrificeState_Idle::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase1::Enter(CSacrifice* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("SacrificeBringer_Ani_P1_Idle")
		.Loop(true)
		.Apply();
}

void CSacrificeState_Idle_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Idle_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase2::Enter(CSacrifice* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("SacrificeBringer_Ani_P2_Idle")
		.Loop(true)
		.BlendDuration(0.5f)
		.Apply();
}

void CSacrificeState_Idle_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Idle_Phase2::Exit(CSacrifice* pOwner)
{
}
