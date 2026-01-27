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

void CDefilerState_Attack::Build_Pattern(CDefiler* pOwner)
{
	CDefiler::DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();
	blackBoard.stateQueue.clear();

	blackBoard.stateQueue.push_back("Attack01_01");
	blackBoard.stateQueue.push_back("Attack01_02");
	blackBoard.isRequestNext = true;
}

void CDefilerState_Attack::ReadySubState()
{
	m_pSubStateMachine->Register_State("Attack01_01",		CDefilerState_Attack_01_01::Create());
	m_pSubStateMachine->Register_State("Attack01_02",		CDefilerState_Attack_01_02::Create());
	m_pSubStateMachine->Register_State("Attack01_01_P2",	CDefilerState_Attack_01_01_P2::Create());
	m_pSubStateMachine->Register_State("Attack01_03",		CDefilerState_Attack_01_03::Create());
	m_pSubStateMachine->Register_State("Attack02",			CDefilerState_Attack_02::Create());
	m_pSubStateMachine->Register_State("Attack03",			CDefilerState_Attack_03::Create());
	m_pSubStateMachine->Register_State("Attack04",			CDefilerState_Attack_04::Create());
	m_pSubStateMachine->Register_State("Attack05",			CDefilerState_Attack_05::Create());
	m_pSubStateMachine->Register_State("Attack06",			CDefilerState_Attack_06::Create());
	m_pSubStateMachine->Register_State("Attack07",			CDefilerState_Attack_07::Create());
	m_pSubStateMachine->Register_State("Attack08_Start",	CDefilerState_Attack_08_01_Start::Create());
	m_pSubStateMachine->Register_State("Attack08_Loop",		CDefilerState_Attack_08_01_Loop::Create());
	m_pSubStateMachine->Register_State("Attack08_End",		CDefilerState_Attack_08_01_End::Create());
	m_pSubStateMachine->Register_State("Attack08_02",		CDefilerState_Attack_08_02::Create());
	m_pSubStateMachine->Register_State("Attack09_Start",	CDefilerState_Attack_09_Start::Create());
	m_pSubStateMachine->Register_State("Attack09_Loop",		CDefilerState_Attack_09_Loop::Create());
	m_pSubStateMachine->Register_State("Attack09_End",		CDefilerState_Attack_09_End::Create());
	m_pSubStateMachine->Register_State("Attack_Grab",		CDefilerState_Attack_Grab::Create());
	m_pSubStateMachine->Register_State("Attack_Summon",		CDefilerState_Attack_Summon::Create());
}

void CDefilerState_Attack::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	Build_Pattern(pOwner);
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

}

void CDefilerState_Attack::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_01::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_01_01::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_02::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_02")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_02::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_01_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01_P2::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01_P2")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_01_P2::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_01_01_P2::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_03::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_03")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_03::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_01_03::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_02::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_02::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_03::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_03::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_03::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_04::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_04::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_04::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_05::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_05::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_05::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_06::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_06::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_06::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_07::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_07::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_07::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Start::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Start::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_08_01_Start::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Loop::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Loop::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_08_01_Loop::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_End::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_End::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_08_01_End::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_02::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_02::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_08_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Start::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Start::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_09_Start::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Loop::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Loop::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_09_Loop::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_End::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_End::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_09_End::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Grab::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Grab::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_Grab::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Summon::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Summon::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Attack_Summon::Exit(CDefiler* pOwner)
{
}
