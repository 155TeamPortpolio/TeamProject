#include "pch.h"
#include "SacrificeState_Death.h"
#include "Sacrifice.h"

void CSacrificeState_Death::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Death_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Death_Phase2::Create());

		__super::Enter(pOwner);
	}

	CSacrifice::PHASE currPhase = pOwner->GetCurrPhase();
	switch (currPhase)
	{
	case Client::CSacrifice::PHASE::PHASE1:
	{
		m_pSubStateMachine->Change_State("Phase1");
	}break;
	case Client::CSacrifice::PHASE::PHASE2:
	{
		m_pSubStateMachine->Change_State("Phase2");
	}break;
	default:
		break;
	}
}

void CSacrificeState_Death::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Death::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Death_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Death_Stay").Loop(false).Speed(1.2f).Apply();

	pOwner->DeactiveAxe();
	pOwner->DeactiveSword();
	pOwner->DeactiveWhip();
	pOwner->DeactiveLaser();
	pOwner->DeactiveEyeLaser();

	pOwner->Set_Hitable(false);
}

void CSacrificeState_Death_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Death_Phase1::Exit(CSacrifice* pOwner)
{

}

void CSacrificeState_Death_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Death_Stay").Loop(false).Speed(1.2f).Apply();

	pOwner->DeactiveAxe();
	pOwner->DeactiveSword();
	pOwner->DeactiveWhip();
	pOwner->DeactiveLaser();
	pOwner->DeactiveEyeLaser();
	pOwner->Set_Hitable(false);
}

void CSacrificeState_Death_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Death_Phase2::Exit(CSacrifice* pOwner)
{
}
