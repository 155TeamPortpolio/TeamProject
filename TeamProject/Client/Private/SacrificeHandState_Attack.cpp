#include "pch.h"
#include "SacrificeHandState_Attack.h"
#include "SacrificeHand.h"

/* Sub States */
#include "SacrificeHandState_Attack_Phase1.h"
#include "SacrificeHandState_Attack_Phase2.h"

void CSacrificeHandState_Attack::Enter(CSacrificeHand* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrificeHand>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeHandState_Attack_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeHandState_Attack_Phase2::Create());
		m_pSubStateMachine->Set_DefaultState("Phase2");
	}

	__super::Enter(pOwner);
}

void CSacrificeHandState_Attack::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeHandState_Attack::Exit(CSacrificeHand* pOwner)
{
}
