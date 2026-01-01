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

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.isRequestNext)
	{
		blackBoard.isRequestNext = false;
		blackBoard.isChainOpen = false;

		if (!blackBoard.stateQueue.empty())
		{
			string nextStateTag = blackBoard.stateQueue.front();
			blackBoard.stateQueue.pop_front();

			blackBoard.currentStateTag = nextStateTag;
			m_pSubStateMachine->Change_State(nextStateTag);
		}
	}
}

void CSacrificeHandState_Attack_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_Phase1::Register_States()
{
	m_pSubStateMachine->Register_State("Attack10_Phase1", CSacrificeHandState_Attack_10_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack11_Phase1", CSacrificeHandState_Attack_11_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack12_Phase1", CSacrificeHandState_Attack_12_Phase1::Create());
}

void CSacrificeHandState_Attack_Phase1::Register_Transitions()
{
}

void CSacrificeHandState_Attack_Phase1::BuildPattern(ATTACK_BLACK_BOARD& blackBoard)
{
	blackBoard.stateQueue.clear();

	blackBoard.stateQueue.push_back("Attack10_Phase1");
	blackBoard.stateQueue.push_back("Attack11_Phase1");
	blackBoard.stateQueue.push_back("Attack12_Phase1");
}

void CSacrificeHandState_Attack_10_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringerHand_Ani_P1_Attack_10").Loop(false).Speed(1.4f).Apply();

	pOwner->SetActive(true);
}

void CSacrificeHandState_Attack_10_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_Attack_10_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_11_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringerHand_Ani_P1_Attack_11").Loop(false).Speed(1.4f).Apply();

	m_IsActiveHand = true;
}

void CSacrificeHandState_Attack_11_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_IsActiveHand && m_fAnimProgress >= 0.2f)
	{
		pOwner->SetActive(false);
		m_IsActiveHand = false;
	}

	if (m_fAnimProgress >= 0.3)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_Attack_11_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_12_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringerHand_Ani_P1_Attack_12").Loop(false).Speed(1.4f).Apply();

	m_IsActiveHand = false;
}

void CSacrificeHandState_Attack_12_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (!m_IsActiveHand && m_fAnimProgress >= 0.1f)
	{
		pOwner->SetActive(true);
		m_IsActiveHand = true;
	}
		
	if (m_fAnimProgress >= 0.5)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

}

void CSacrificeHandState_Attack_12_Phase1::Exit(CSacrificeHand* pOwner)
{
	pOwner->Set_Alive(false);
	pOwner->SetActive(false);
}
