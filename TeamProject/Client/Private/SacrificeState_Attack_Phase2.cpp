#include "pch.h"
#include "SacrificeState_Attack_Phase2.h"
#include "Sacrifice.h"
#include "Helper_Func.h"

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
	{
		_uint iRandIndex = Helper::Get_Random_Int(0, 1);
		if (0 == iRandIndex || blackBoard.currentStateTag == "OverDrive_Loop")
			pOwner->Idle();
		else
			pOwner->Evade();
	}
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
	m_pSubStateMachine->Register_State("Attack08_Phase2", CSacrificeState_Attack_08_Phase2::Create());

	m_pSubStateMachine->Register_State("Attack_Charge_Start_Phase2", CSacrificeState_Attack_Charge_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_Loop_Phase2", CSacrificeState_Attack_Charge_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_Start_Phase2",CSacrificeState_Attack_Charge_U_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_Loop_Phase2", CSacrificeState_Attack_Charge_U_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_End_Phase2",CSacrificeState_Attack_Charge_U_End_Phase2::Create());

	m_pSubStateMachine->Register_State("OverDrive_Start", CSacrificeState_OverDrive_Release_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Loop", CSacrificeState_OverDrive_Release_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_End", CSacrificeState_OverDrive_Release_End_Phase2::Create());
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
		if (!pOwner->IsOverDriveCharged())
		{
			blackBoard.stateQueue.push_back("OverDrive_Start");
			blackBoard.stateQueue.push_back("OverDrive_Loop");
		}
		else
		{
			blackBoard.stateQueue.push_back("OverDrive_Attack01");
			blackBoard.stateQueue.push_back("OverDrive_Attack02");
			blackBoard.stateQueue.push_back("OverDrive_Attack03");
		}
	}
	else
	{
		_uint iRandIndex = Helper::Get_Random_Int(0, 4);
		iRandIndex = 5;
		switch (iRandIndex)
		{
		case 0:
		{
			blackBoard.stateQueue.push_back("Attack01_Phase2");
			blackBoard.stateQueue.push_back("Attack02_Phase2");

		}break;
		case 1:
		{
			blackBoard.stateQueue.push_back("Attack_Charge_Start_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_Loop_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_Start_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_Loop_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_End_Phase2");
		}break;
		case 2:
		{
			blackBoard.stateQueue.push_back("Attack04_Phase2");
		}break;
		case 3:
		{
			blackBoard.stateQueue.push_back("Attack03_Phase2");
		}break;
		case 4:
		{
			blackBoard.stateQueue.push_back("Attack08_Phase2");
		}break;
		case 5:
		{
			blackBoard.stateQueue.push_back("Attack05_1_Phase2");
			blackBoard.stateQueue.push_back("Attack05_Phase2");
		}break;
		default:
			break;
		}
	}
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
	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->DeactiveWhip();
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
	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
		pOwner->DeactiveWhip();
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
	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
		pOwner->DeactiveWhip();
	}
}

void CSacrificeState_Attack_03_Phase2::Exit(CSacrifice* pOwner)
{
	pOwner->DeactiveWhip();
}

void CSacrificeState_Attack_04_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack04").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_04_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_04_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_05").Loop(false).Speed(1.2f).Apply();

}

void CSacrificeState_Attack_05_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.45f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_05_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_1_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_05_1").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_05_1_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_05_1_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_08_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_08").Loop(false).Speed(1.2f).Apply();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
}

void CSacrificeState_Attack_08_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (!m_IsAttackStart && m_fAnimProgress >= 0.1f)
	{
		pOwner->ActiveAxe();
		m_IsAttackStart = true;
	}
	
	if (!m_IsAttackEnd && m_fAnimProgress >= 0.85f)
	{
		pOwner->DeactiveAxe();
		m_IsAttackEnd = true;
	}

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_08_Phase2::Exit(CSacrifice* pOwner)
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

	pOwner->OverDrive_Start();
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress>=0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
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

	if (m_fStateTime >= 3.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->SetOverDriveCharged(true);
	}
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_End_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Start").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_End_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_End_Phase2::Exit(CSacrifice* pOwner)
{
}


void CSacrificeState_OverDrive_Release_Attack01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack01").Loop(false).Speed(1.2f).Apply();

	pOwner->OverDrive_Attack1();
	pOwner->ActiveSword();
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack02").Loop(false).Speed(1.2f).Apply();

	pOwner->OverDrive_Attack2();
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack03").Loop(false).Speed(1.2f).Apply();

	m_IsHandSpawn = false;
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (!m_IsHandSpawn && m_fAnimProgress >= 0.25f)
	{
		pOwner->OverDrive_Attack3();
		m_IsHandSpawn = true;
	}


	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->DeactiveSword();
		pOwner->SetOverDrive(false);
		pOwner->SetOverDriveCharged(false);
	}
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Exit(CSacrifice* pOwner)
{
}
