#include "pch.h"
#include "SacrificeState_Groggy.h"
#include "Sacrifice.h"
#include "SacrificeState_Groggy_Phase1.h"
#include "SacrificeState_Groggy_Phase2.h"

void CSacrificeState_Groggy::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		/* States */
		m_pSubStateMachine->Register_State("Phase1",CSacrificeState_Groggy_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Groggy_Phase2::Create());

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

void CSacrificeState_Groggy::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Groggy::Exit(CSacrifice* pOwner)
{
}
