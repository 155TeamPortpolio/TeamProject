#include "pch.h"
#include "SacrificeState_Walk.h"
#include "Sacrifice.h"
#include "CharacterController.h"

/* Sub States */
#include "SacrificeState_Walk_Phase1.h"
#include "SacrificeState_Walk_Phase2.h"

void CSacrificeState_Walk::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Walk_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Walk_Phase2::Create());

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

void CSacrificeState_Walk::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	
	pOwner->RotateToTarget(dt, 5.f);
	pOwner->MoveByRootMotion(dt);

	if (m_fStateTime >= 1.f)
		pOwner->Idle();
}

void CSacrificeState_Walk::Exit(CSacrifice* pOwner)
{
}
