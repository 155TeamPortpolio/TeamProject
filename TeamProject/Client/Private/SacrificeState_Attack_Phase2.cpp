#include "pch.h"
#include "SacrificeState_Attack_Phase2.h"
#include "Sacrifice.h"

void CSacrificeState_Attack_Phase2::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();
		Register_Transitions();
	}
}

void CSacrificeState_Attack_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase2::Register_States()
{
}

void CSacrificeState_Attack_Phase2::Register_Transitions()
{
}
