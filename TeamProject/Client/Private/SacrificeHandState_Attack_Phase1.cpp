#include "pch.h"
#include "SacrificeHand.h"
#include "SacrificeHandState_Attack_Phase1.h"

void CSacrificeHandState_Attack_Phase1::Enter(CSacrificeHand* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrificeHand>::Create();

		Register_States();

	}

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	BuildPattern(blackBoard);
	blackBoard.isRequestNext = true;

	__super::Enter(pOwner);
}

void CSacrificeHandState_Attack_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeHandState_Attack_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_Phase1::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01_Phase1", CSacrificeHandState_Attack_01_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase1", CSacrificeHandState_Attack_02_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase1", CSacrificeHandState_Attack_03_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_Phase1", CSacrificeHandState_Attack_04_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase1", CSacrificeHandState_Attack_05_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack06_Phase1", CSacrificeHandState_Attack_06_Phase1::Create());

	m_pSubStateMachine->Register_State("Attack10_Phase1", CSacrificeHandState_Attack_10_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack11_Phase1", CSacrificeHandState_Attack_11_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack12_Phase1", CSacrificeHandState_Attack_12_Phase1::Create());
}

void CSacrificeHandState_Attack_Phase1::Register_Transitions()
{
}

void CSacrificeHandState_Attack_Phase1::BuildPattern(ATTACK_BLACK_BOARD& blackBoard)
{
	blackBoard.stateQueue.push_back("Attack07_Phase1");
	blackBoard.stateQueue.push_back("Attack02_Phase1");
	blackBoard.stateQueue.push_back("Attack08_Phase1");
}

void CSacrificeHandState_Attack_01_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_01_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_01_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_02_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_02_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_02_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_03_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_03_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_03_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_04_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_04_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_04_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_05_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_05_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_05_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_06_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_06_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_06_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_10_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_10_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_10_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_11_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_11_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_11_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_12_Phase1::Enter(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_12_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Attack_12_Phase1::Exit(CSacrificeHand* pOwner)
{
}
