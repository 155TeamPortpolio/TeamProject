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

void CDefilerState_Attack::Build_Pattern(CDefiler* pOwner, _int Type)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();
	blackBoard.patternTransition.clear();

	switch (Type)
	{
	case 0 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_01",0.f,0.41f });
		blackBoard.patternTransition.push_back({ "Attack01_02",0.19f,1.f });
		break;
	}
	case 1 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_03",0.f,1.f });
		break;
	}
	case 2 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_01_P2",0.f,1.f });
		break;
	}
	case 3 :
	{
		blackBoard.patternTransition.push_back({ "Attack02",0.f,1.f });
		break;
	}
	case 4 :
	{
		blackBoard.patternTransition.push_back({ "Attack03",0.f,1.f });
		break;
	}
	case 5 :
	{
		blackBoard.patternTransition.push_back({ "Attack04",0.f,1.f });
		break;
	}
	case 6 :
	{
		blackBoard.patternTransition.push_back({ "Attack05",0.f,1.f });
		break;
	}
	case 7 :
	{
		blackBoard.patternTransition.push_back({ "Attack06",0.f,1.f });
		break;
	}
	case 8 :
	{
		blackBoard.patternTransition.push_back({ "Attack07",0.f,1.f });
		break;
	}
	case 9 :
	{
		blackBoard.patternTransition.push_back({ "Attack08_01_Start",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack08_01_Loop",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack08_01_End",0.f,1.f });
	
		break;
	}
	case 10 :
	{
		blackBoard.patternTransition.push_back({ "Attack09_Start",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack09_Loop",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack09_End",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack01_01_P2",0.f,1.f });
		break;
	}
	case 11 :
	{
		blackBoard.patternTransition.push_back({ "Attack_Grab",0.f,1.f });
		break;
	}
	case 12 :
	{
		blackBoard.patternTransition.push_back({ "Attack_Summon",0.f,1.f });
		break;
	}
	default:
		break;
	}

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
	auto& blackboard = pOwner->GetBlackBoard();

	Build_Pattern(pOwner, blackboard.patternIndex);
	//blackboard.patternIndex = 6;// clamp(++blackboard.patternIndex, 0, m_maxPattern);

	if (!blackboard.patternTransition.empty())
	{
		blackboard.ReservePattern();
		blackboard.ResetNextFlag();
		m_pSubStateMachine->Change_State(blackboard.reservedPattern.nextPattern);
	}
}

void CDefilerState_Attack::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	auto& blackboard = pOwner->GetBlackBoard();

	if (blackboard.isRequestNext)
	{
		blackboard.isRequestNext = false;

		if (!blackboard.patternTransition.empty())
		{
			blackboard.ReservePattern();
			m_pSubStateMachine->Change_State(blackboard.reservedPattern.nextPattern);
		}
		else
		{
			pOwner->Get_MainStateMachine()->Set_Trigger("Idle");
		}
	}
}

void CDefilerState_Attack::ComboTransition(CDefiler* pOwner)
{
	auto& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= blackBoard.reservedPattern.animEndProgress)
		blackBoard.isRequestNext = true;
}

void CDefilerState_Attack::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_02")
		.Speed(1.f)
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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

	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01_P2")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_03")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_02")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_03")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_04")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_Fierce();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_05")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();

	pOwner->Change_CollisionMask();
}

void CDefilerState_Attack_05::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_05::Exit(CDefiler* pOwner)
{
	pOwner->Release_CollisionMask();
}

void CDefilerState_Attack_06::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_06")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_07")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_Start")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_Loop")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_End")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_02")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_Start")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_Loop")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_End")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_Grab_01")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_Summon")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
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

void CDefilerState_Attack_Evade::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Evade::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_Evade::Exit(CDefiler* pOwner)
{
}
