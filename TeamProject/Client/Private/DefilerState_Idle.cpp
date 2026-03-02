#include "pch.h"
#include "DefilerState_Idle.h"
#include "Defiler.h"
#include "StateMachine.h"

void CDefilerState_Idle::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Idle_Loop")
		.Speed(1.f)
		.Loop(true)
		.Apply();

	if (!blackBoard.patternTransition.empty() && blackBoard.patternTransition.front().nextPattern == "Attack_Barrier") {
		IdleDuration = .5f;
	}
}

void CDefilerState_Idle::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	/* Idle */
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	m_IdleElapsedTime += dt;
	if (m_IdleElapsedTime >= IdleDuration)
	{
		if (blackBoard.ForceIDLE) {
			m_IdleElapsedTime = 0;
			blackBoard.ForceIDLE = false;
		}
		else {
			pOwner->Get_MainStateMachine()->Set_Trigger("Idle_To_Attack");
		}
	}
}

void CDefilerState_Idle::Exit(CDefiler* pOwner)
{
	m_IdleElapsedTime = 0;
	IdleDuration = .2f;
}
