#include "pch.h"
#include "SacrificeState_Evade.h"
#include "Sacrifice.h"

/* Component */
#include "CharacterController.h"

/* Sub States */
#include "SacrificeState_Evade_Phase1.h"
#include "SacrificeState_Evade_Phase2.h"

void CSacrificeState_Evade::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Evade_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Evade_Phase2::Create());

		__super::Enter(pOwner);
	}

	CSacrifice::PHASE currPhase = pOwner->GetCurrPhase();
	switch (currPhase)
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

void CSacrificeState_Evade::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		pOwner->Idle();
	}
	else
	{
		pOwner->MoveByRootMotion(dt);
	}
}

void CSacrificeState_Evade::Exit(CSacrifice* pOwner)
{
}
