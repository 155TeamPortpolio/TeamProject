#include "pch.h"
#include "DefilerState_Attack.h"
#include "Defiler.h"
#include "StateMachine.h"

CDefilerState_Attack* CDefilerState_Attack::Create()
{
	CDefilerState_Attack* pInstance = new CDefilerState_Attack();
	pInstance->m_pSubStateMachine = CStateMachine<CDefiler>::Create();
	pInstance->ReadySubState();

	return pInstance;
}

void CDefilerState_Attack::ComboTransition(CDefiler* pOwner)
{
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
	else if (blackBoard.ConsumeChain()) {
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CDefilerState_Attack::Build_Pattern(CDefiler* pOwner, _int Type)
{
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();
	blackBoard.stateQueue.clear();
	blackBoard.OnTarget();

	switch (Type)
	{
	case 0 :
	{
		blackBoard.stateQueue.push_back("Attack01_01");
		blackBoard.progressQueue.push_back(0.18);
		blackBoard.stateQueue.push_back("Attack01_02");
		break;
	}
	case 1 :
	{
		blackBoard.stateQueue.push_back("Attack01_03");
		break;
	}
	case 2 :
	{
		blackBoard.stateQueue.push_back("Attack01_01_P2");
		break;
	}
	case 3 :
	{
		blackBoard.stateQueue.push_back("Attack02");
		break;
	}
	case 4 :
	{
		blackBoard.stateQueue.push_back("Attack03");
		break;
	}
	case 5 :
	{
		blackBoard.stateQueue.push_back("Attack04");
		break;
	}
	case 6 :
	{
		blackBoard.stateQueue.push_back("Attack05");
		break;
	}
	case 7 :
	{
		blackBoard.stateQueue.push_back("Attack06");
		break;
	}
	case 8 :
	{
		blackBoard.stateQueue.push_back("Attack07");
		break;
	}
	case 9 :
	{
		blackBoard.stateQueue.push_back("Attack08_01_Start");
		blackBoard.stateQueue.push_back("Attack08_01_Loop");
		blackBoard.stateQueue.push_back("Attack08_01_End");
		break;
	}
	case 10 :
	{
		blackBoard.stateQueue.push_back("Attack09_Start");
		blackBoard.stateQueue.push_back("Attack09_Loop");		
		blackBoard.stateQueue.push_back("Attack09_End");
		blackBoard.stateQueue.push_back("Attack01_01_P2");
		break;
	}
	case 11 :
	{
		blackBoard.stateQueue.push_back("Attack_Grab");
		break;
	}
	case 12 :
	{
		blackBoard.stateQueue.push_back("Attack_Summon");
		break;
	}
	default:
		break;
	}
	

	//
	//blackBoard.stateQueue.push_back("Attack02");
	//blackBoard.stateQueue.push_back("Attack03");
	//blackBoard.stateQueue.push_back("Attack04");
	//blackBoard.stateQueue.push_back("Attack05");
	//blackBoard.stateQueue.push_back("Attack06");
	//blackBoard.stateQueue.push_back("Attack07");
	//blackBoard.stateQueue.push_back("Attack08_01_Start");
	//blackBoard.stateQueue.push_back("Attack08_01_Loop");
	//blackBoard.stateQueue.push_back("Attack08_01_End");
	//blackBoard.stateQueue.push_back("Attack09_Start");
	//blackBoard.stateQueue.push_back("Attack09_Loop");
	//blackBoard.stateQueue.push_back("Attack09_End");
	//blackBoard.stateQueue.push_back("Attack_Grab");
	//blackBoard.stateQueue.push_back("Attack_Summon");
	blackBoard.isRequestNext = true;
}

void CDefilerState_Attack::ReadySubState()
{
	m_pSubStateMachine->Register_State("Attack01_01",			CDefilerState_Attack_01_01::Create());
	m_pSubStateMachine->Register_State("Attack01_02",			CDefilerState_Attack_01_02::Create());
	m_pSubStateMachine->Register_State("Attack01_01_P2",		CDefilerState_Attack_01_01_P2::Create());
	m_pSubStateMachine->Register_State("Attack01_03",			CDefilerState_Attack_01_03::Create());
	m_pSubStateMachine->Register_State("Attack02",				CDefilerState_Attack_02::Create());
	m_pSubStateMachine->Register_State("Attack03",				CDefilerState_Attack_03::Create());
	m_pSubStateMachine->Register_State("Attack04",				CDefilerState_Attack_04::Create());
	m_pSubStateMachine->Register_State("Attack05",				CDefilerState_Attack_05::Create());
	m_pSubStateMachine->Register_State("Attack06",				CDefilerState_Attack_06::Create());
	m_pSubStateMachine->Register_State("Attack07",				CDefilerState_Attack_07::Create());
	m_pSubStateMachine->Register_State("Attack08_01_Start",		CDefilerState_Attack_08_01_Start::Create());
	m_pSubStateMachine->Register_State("Attack08_01_Loop",		CDefilerState_Attack_08_01_Loop::Create());
	m_pSubStateMachine->Register_State("Attack08_01_End",		CDefilerState_Attack_08_01_End::Create());
	m_pSubStateMachine->Register_State("Attack08_02",			CDefilerState_Attack_08_02::Create());
	m_pSubStateMachine->Register_State("Attack09_Start",		CDefilerState_Attack_09_Start::Create());
	m_pSubStateMachine->Register_State("Attack09_Loop",			CDefilerState_Attack_09_Loop::Create());
	m_pSubStateMachine->Register_State("Attack09_End",			CDefilerState_Attack_09_End::Create());
	m_pSubStateMachine->Register_State("Attack_Grab",			CDefilerState_Attack_Grab::Create());
	m_pSubStateMachine->Register_State("Attack_Summon",			CDefilerState_Attack_Summon::Create());
}

void CDefilerState_Attack::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard(); 
	Build_Pattern(pOwner, blackBoard.pattern);
	blackBoard.pattern++;
	blackBoard.pattern = clamp(blackBoard.pattern, 0, m_maxPattern);
}

void CDefilerState_Attack::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
		pOwner->Get_MainStateMachine()->Set_Trigger("Idle");
	}
}

void CDefilerState_Attack::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01::Enter(CDefiler* pOwner)
{
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01")
		.Speed(1.f)
		.UseFinalLocalPose(true)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_01::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_01_01::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_02::Enter(CDefiler* pOwner)
{
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.PopStartProgress();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_02")
		.Speed(1.f)
		.StartAt(blackBoard.currentStartProgress)
		.Loop(false)
		.Apply();

}

void CDefilerState_Attack_01_02::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_01_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01_P2::Enter(CDefiler* pOwner)
{

	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01_P2")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_01_P2::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_01_01_P2::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_03::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_03")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_03::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_01_03::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_02::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_02")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_02::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_03::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_03")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_03::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_03::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_04::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_04")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_04::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_04::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_05::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_05")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_05::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_05::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_06::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_06")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_06::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_06::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_07::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_07")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_07::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_07::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Start::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_Start")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_08_01_Start::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_01_Start::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Loop::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_Loop")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_08_01_Loop::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_01_Loop::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_End::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_End")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_08_01_End::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_01_End::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_02::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_02")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_08_02::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Start::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_Start")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_09_Start::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_09_Start::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Loop::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_Loop")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_09_Loop::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_09_Loop::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_End::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_End")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_09_End::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_09_End::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Grab::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_Grab_01")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_Grab::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_Grab::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Summon::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_Summon")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_Summon::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_Summon::Exit(CDefiler* pOwner)
{
}
