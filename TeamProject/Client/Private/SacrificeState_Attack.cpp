#include "pch.h"
#include "SacrificeState_Attack.h"
#include "Sacrifice.h"
#include "Helper_Func.h"
#include "CharacterController.h"

/* State */
#include "SacrificeState_Attack_Phase1.h"
#include "SacrificeState_Attack_Phase2.h"

void CSacrificeState_Attack::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		/* States */
		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Attack_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Attack_Phase2::Create());

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

void CSacrificeState_Attack::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Attack::Exit(CSacrifice* pOwner)
{
}