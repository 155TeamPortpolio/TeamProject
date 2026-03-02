#include "pch.h"
#include "SacrificeState_Groggy_Phase1.h"
#include "Sacrifice.h"

void CSacrificeState_Groggy_Phase1::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Groggy_Start_F_Phase1",CSacrificeState_Groggy_Start_Front_Phase1::Create());
		m_pSubStateMachine->Register_State("Groggy_Start_B_Phase1",CSacrificeState_Groggy_Start_Back_Phase1::Create());
		m_pSubStateMachine->Register_State("Groggy_Loop_Phase1",CSacrificeState_Groggy_Loop_Phase1::Create());
		m_pSubStateMachine->Register_State("Groggy_End_Phase1",CSacrificeState_Groggy_End_Phase1::Create());
	}

	pOwner->DeactiveAxe();
	pOwner->DeactiveSword();
	pOwner->DeactiveWhip();
	pOwner->DeactiveEyeLaser();
	pOwner->DeactiveLaser();

	BuildPattern(pOwner);
	__super::Enter(pOwner);
}

void CSacrificeState_Groggy_Phase1::Update(CSacrifice* pOwner, _float dt)
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

void CSacrificeState_Groggy_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Groggy_Phase1::BuildPattern(CSacrifice* pOwner)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();

	blackBoard.stateQueue.clear();

	_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
	_vector3 vTargetDir = targetInfo.vDirToTarget;

	/* 타겟이 뒤에 있음 */
	if (vLook.Dot(vTargetDir) < 0.f)
		blackBoard.stateQueue.push_back("Groggy_Start_B_Phase1");
	else
		blackBoard.stateQueue.push_back("Groggy_Start_F_Phase1");

	blackBoard.stateQueue.push_back("Groggy_Loop_Phase1");
	blackBoard.stateQueue.push_back("Groggy_End_Phase1");

	blackBoard.isRequestNext = true;
}

void CSacrificeState_Groggy_Start_Front_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Debuff_Stun_Start_Front").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Groggy_Start_Front_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	pOwner->MoveByRootMotion(dt);

	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

}

void CSacrificeState_Groggy_Start_Front_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Groggy_Start_Back_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Debuff_Stun_Start_Back").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Groggy_Start_Back_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	pOwner->MoveByRootMotion(dt);

	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Groggy_Start_Back_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Groggy_Loop_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Debuff_Stun_Loop").Loop(true).Speed(1.f).Apply();
}

void CSacrificeState_Groggy_Loop_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	pOwner->MoveByRootMotion(dt);

	if (pOwner->GetStatus().iGroggyValue <= 0)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Groggy_Loop_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Groggy_End_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Debuff_Stun_End").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Groggy_End_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);

	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Groggy_End_Phase1::Exit(CSacrifice* pOwner)
{
}
