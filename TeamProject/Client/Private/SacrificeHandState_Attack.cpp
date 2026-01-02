#include "pch.h"
#include "SacrificeHandState_Attack.h"
#include "SacrificeHand.h"

void CSacrificeHandState_Attack::Enter(CSacrificeHand* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrificeHand>::Create();

		Register_States();
		__super::Enter(pOwner);
	}

	BuildPattern(pOwner);
}

void CSacrificeHandState_Attack::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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

	if (blackBoard.isChainOpen && blackBoard.stateQueue.empty())
		pOwner->SetVisable(false);
}

void CSacrificeHandState_Attack::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack::Register_States()
{
	/* Phase1 Attack */
	m_pSubStateMachine->Register_State("Attack01_Phase1", CSacrificeHandState_Attack_01_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase1", CSacrificeHandState_Attack_02_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase1", CSacrificeHandState_Attack_03_Phase1::Create());

	/* OverDrive Start */
	m_pSubStateMachine->Register_State("OverDrive_Start", CSacrificeHandState_OverDrive_Release_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Loop", CSacrificeHandState_OverDrive_Release_Loop_Phase2::Create());
}

void CSacrificeHandState_Attack::BuildPattern(CSacrificeHand* pOwner)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.stateQueue.clear();

	switch (blackBoard.eCurrPattern)
	{
	case Client::CSacrificeHand::PATTERN::PHASE1:
	{
		blackBoard.stateQueue.push_back("Attack01_Phase1");
		blackBoard.stateQueue.push_back("Attack02_Phase1");
		blackBoard.stateQueue.push_back("Attack03_Phase1");
	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_START:
	{
		blackBoard.stateQueue.push_back("OverDrive_Start");
		blackBoard.stateQueue.push_back("OverDrive_Loop");
	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_ATTACK01:
	{

	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_ATTACK02:
	{

	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_ATTACK03:
		break;
	default:
		break;
	}

	if (!blackBoard.stateQueue.empty())
		blackBoard.isRequestNext = true;
}

void CSacrificeHandState_Attack_01_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringerHand_Ani_P1_Attack_10").Loop(false).Speed(1.4f).Apply();

	pOwner->SetVisable(true);
}

void CSacrificeHandState_Attack_01_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_Attack_01_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_02_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringerHand_Ani_P1_Attack_11").Loop(false).Speed(1.4f).Apply();

	m_IsActiveHand = true;
}

void CSacrificeHandState_Attack_02_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_IsActiveHand && m_fAnimProgress >= 0.2f)
	{
		pOwner->SetVisable(false);
		m_IsActiveHand = false;
	}

	if (m_fAnimProgress >= 0.3)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_Attack_02_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_03_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringerHand_Ani_P1_Attack_12").Loop(false).Speed(1.4f).Apply();

	m_IsActiveHand = false;
}

void CSacrificeHandState_Attack_03_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (!m_IsActiveHand && m_fAnimProgress >= 0.1f)
	{
		pOwner->SetVisable(true);
		m_IsActiveHand = true;
	}

	if (m_fAnimProgress >= 0.5)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_Attack_03_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringerHand_Ani_P2_OverDrive_Charge_Start_New").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	
	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Loop_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringerHand_Ani_P2_OverDrive_Charge_Loop").Loop(true).Speed(1.2f).Apply();
}

void CSacrificeHandState_OverDrive_Release_Loop_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fStateTime >= 1.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeHandState_OverDrive_Release_Loop_Phase2::Exit(CSacrificeHand* pOwner)
{
}
