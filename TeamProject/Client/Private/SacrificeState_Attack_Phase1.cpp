#include "pch.h"
#include "SacrificeState_Attack_Phase1.h"
#include "Sacrifice.h"
#include "CharacterController.h"
#include "Helper_Func.h"

void CSacrificeState_Attack_Phase1::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();
		Register_Transitions();
	}

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	BuildPattern(blackBoard);
	blackBoard.isRequestNext = true;

	__super::Enter(pOwner);
}

void CSacrificeState_Attack_Phase1::Update(CSacrifice* pOwner, _float dt)
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

void CSacrificeState_Attack_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase1::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01_Phase1", CSacrificeState_Attack_01_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase1", CSacrificeState_Attack_02_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase1", CSacrificeState_Attack_03_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_1_Phase1", CSacrificeState_Attack_04_1_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_2_Phase1", CSacrificeState_Attack_04_2_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase1", CSacrificeState_Attack_05_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack06_Phase1", CSacrificeState_Attack_06_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack07_Phase1", CSacrificeState_Attack_07_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack08_Phase1", CSacrificeState_Attack_08_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack09_Phase1", CSacrificeState_Attack_09_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack10_Phase1", CSacrificeState_Attack_10_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack11_Phase1", CSacrificeState_Attack_11_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack12_Phase1", CSacrificeState_Attack_12_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack_Turn_Phase1", CSacrificeState_Attack_Turn_Phase1::Create());
}

void CSacrificeState_Attack_Phase1::Register_Transitions()
{
}

void CSacrificeState_Attack_Phase1::BuildPattern(ATTACK_BLACK_BOARD& blackBoard)
{
	_uint iRandIndex = Helper::Get_Random_Int(0, 1);
	iRandIndex = 0;
	switch (iRandIndex)
	{
	case 0:
	{
		blackBoard.stateQueue.push_back("Attack07_Phase1");
		blackBoard.stateQueue.push_back("Attack02_Phase1");
		blackBoard.stateQueue.push_back("Attack08_Phase1");
	}break;
	case 1:
	{
		blackBoard.stateQueue.push_back("Attack07_Phase1");
		blackBoard.stateQueue.push_back("Attack02_Phase1");
		blackBoard.stateQueue.push_back("Attack03_Phase1");
	}break;
	case 2:
	{
		blackBoard.stateQueue.push_back("Attack01_Phase1");
		blackBoard.stateQueue.push_back("Attack02_Phase1");

	}break;
	case 3:
	{

	}break;
	case 4:
	{

	}break;
	default:
		break;
	}
}


void CSacrificeState_Attack_01_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_01").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_01_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.3f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_Attack_01_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_02_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_02").Loop(false).Speed(1.2f).Apply();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_fAnimProgress = 0.f;
}

void CSacrificeState_Attack_02_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.2f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}

	if (!m_IsAttackStart && m_fAnimProgress >= 0.05f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveSword();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.15f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveSword();
	}
}

void CSacrificeState_Attack_02_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_03_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_03").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_03_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.2f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}
}

void CSacrificeState_Attack_03_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_04_1_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_04_1").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_04_1_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_04_1_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_04_2_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_04_2").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_04_2_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_04_2_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_05").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_05_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_05_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_06_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_06").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_06_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_06_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_07_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_07").Loop(false).Speed(1.2f).Apply();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_fAnimProgress = 0.f;
}

void CSacrificeState_Attack_07_Phase1::Update(CSacrifice* pOwner, _float dt)
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

	if (!m_IsAttackStart && m_fAnimProgress >= 0.05f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveSword();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.6f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveSword();
	}
}

void CSacrificeState_Attack_07_Phase1::Exit(CSacrifice* pOwner)
{
	static_cast<CSacrifice*>(pOwner)->DeactiveSword();
}

void CSacrificeState_Attack_08_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_08").Loop(false).Speed(1.2f).Apply();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
}

void CSacrificeState_Attack_08_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.75f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}

	if (!m_IsAttackStart && m_fAnimProgress >= 0.06f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveAxe();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.5f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveAxe();
	}

}

void CSacrificeState_Attack_08_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_09_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_09").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_09_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_09_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_10_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_10").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_10_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_10_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_11_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_11").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_11_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_11_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_12_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_12").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_12_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_12_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Turn_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_Turn").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_Turn_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_Turn_Phase1::Exit(CSacrifice* pOwner)
{
}

