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

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	BuildPattern(pOwner, blackBoard);
	blackBoard.isRequestNext = true;

	__super::Enter(pOwner);
}

void CSacrificeState_Attack_Phase2::Update(CSacrifice* pOwner, _float dt)
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

	if (blackBoard.isChainOpen && blackBoard.stateQueue.empty())
		pOwner->Idle();
}

void CSacrificeState_Attack_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase2::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01_Phase2",CSacrificeState_Attack_01_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase2",CSacrificeState_Attack_02_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase2",CSacrificeState_Attack_03_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack04_Phase2",CSacrificeState_Attack_04_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase2",CSacrificeState_Attack_05_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack05_1_Phase2",CSacrificeState_Attack_05_1_Phase2::Create());

	m_pSubStateMachine->Register_State("Attack_Charge_Start_Phase2", CSacrificeState_Attack_Charge_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_Loop_Phase2", CSacrificeState_Attack_Charge_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_Start_Phase2",CSacrificeState_Attack_Charge_U_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_Loop_Phase2", CSacrificeState_Attack_Charge_U_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_End_Phase2",CSacrificeState_Attack_Charge_U_End_Phase2::Create());

	m_pSubStateMachine->Register_State("OverDrive_Start", CSacrificeState_OverDrive_Release_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Loop", CSacrificeState_OverDrive_Release_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack01", CSacrificeState_OverDrive_Release_Attack01_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack02", CSacrificeState_OverDrive_Release_Attack02_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack03", CSacrificeState_OverDrive_Release_Attack03_Phase2::Create());
}

void CSacrificeState_Attack_Phase2::Register_Transitions()
{
}

void CSacrificeState_Attack_Phase2::BuildPattern(CSacrifice* pOwner, ATTACK_BLACK_BOARD& blackBoard)
{
	if (pOwner->IsOverDrive())
	{
		blackBoard.stateQueue.push_back("OverDrive_Start");
		blackBoard.stateQueue.push_back("OverDrive_Loop");
		blackBoard.stateQueue.push_back("OverDrive_Attack01");
		blackBoard.stateQueue.push_back("OverDrive_Attack02");
		blackBoard.stateQueue.push_back("OverDrive_Attack03");
	}
	else
	{
		blackBoard.stateQueue.push_back("Attack01_Phase2");
		blackBoard.stateQueue.push_back("Attack02_Phase2");
		blackBoard.stateQueue.push_back("Attack03_Phase2");
	}

	//blackBoard.stateQueue.push_back("Attack_Charge_Start_Phase2");
	//blackBoard.stateQueue.push_back("Attack_Charge_Loop_Phase2");
	//blackBoard.stateQueue.push_back("Attack_Charge_U_Start_Phase2");
	//blackBoard.stateQueue.push_back("Attack_Charge_U_Loop_Phase2");
	//blackBoard.stateQueue.push_back("Attack_Charge_U_End_Phase2");

	//blackBoard.stateQueue.push_back("OverDrive_Attack01");
	//blackBoard.stateQueue.push_back("OverDrive_Attack02");
	//blackBoard.stateQueue.push_back("OverDrive_Attack03");

}

void CSacrificeState_Attack_01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_01").Loop(false).Speed(1.4f).Apply();

	m_IsAttackStart = false;
}

void CSacrificeState_Attack_01_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	if (!m_IsAttackStart && m_fAnimProgress >= 0.1f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveWhip();
	}

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	
	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
		{
			pOwner->DeactiveWhip();
			blackBoard.isChainOpen = true;
		}
	}
	else
	{
		if (m_fAnimProgress >= 0.8f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;

			pOwner->DeactiveWhip();
		}
	}
}

void CSacrificeState_Attack_01_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_02_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_02").Loop(false).Speed(1.4f).Apply();
	pOwner->ActiveWhip();
}

void CSacrificeState_Attack_02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
		{
			blackBoard.isChainOpen = true;
			pOwner->DeactiveWhip();
		}
	}
	else
	{
		if (m_fAnimProgress >= 0.7f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
			pOwner->DeactiveWhip();
		}
	}
}

void CSacrificeState_Attack_02_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_03_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_03").Loop(false).Speed(1.4f).Apply();
	pOwner->ActiveWhip();
}

void CSacrificeState_Attack_03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
		{
			pOwner->DeactiveWhip();
			blackBoard.isChainOpen = true;
		}
	}
	else
	{
		if (m_fAnimProgress >= 0.8f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
			pOwner->DeactiveWhip();
		}
	}
}

void CSacrificeState_Attack_03_Phase2::Exit(CSacrifice* pOwner)
{
	pOwner->DeactiveWhip();
}

void CSacrificeState_Attack_04_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack04").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_04_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_04_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_05").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_05_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_05_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_1_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_05_1").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_05_1_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_05_1_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_Start").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.9f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_Attack_Charge_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Charge_Loop").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fStateTime >= 1.5f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_Attack_Charge_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_U_Start").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.9f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_U_Loop").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fStateTime >= 2.f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
   	pAnimator->Change_Animation("Take 001Monster_SacrificeBringer_Ani_P2_Charge_U_End").Loop(false).Speed(1.6f).Apply();
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Charge_Start_New").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
		blackBoard.isChainOpen = true;
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Charge_Loop").Loop(true).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fStateTime >= 1.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack01").Loop(false).Speed(1.2f).Apply();

	pOwner->ActiveSword();
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.8f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack02").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.8f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack03").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.8f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Exit(CSacrifice* pOwner)
{
	pOwner->DeactiveSword();
	pOwner->DeactiveOverDrive();
}
