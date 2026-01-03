#include "pch.h"
#include "SacrificeState_Hit.h"
#include "Sacrifice.h"

/* Sub States */
#include "SacrificeState_Hit_Phase1.h"
#include "SacrificeState_Hit_Phase2.h"

void CSacrificeState_Hit::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1",CSacrificeState_Hit_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2",CSacrificeState_Hit_Phase2::Create());

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

void CSacrificeState_Hit::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Hit::Exit(CSacrifice* pOwner)
{
}
