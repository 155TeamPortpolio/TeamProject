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
}

void CDefilerState_Idle::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	/* Idle */
	m_IdleElapsedTime += dt;
	if (m_IdleElapsedTime >= IdleDuration)
	{
		pOwner->Get_MainStateMachine()->Set_Trigger("Idle_To_Attack");
	}
}

void CDefilerState_Idle::Exit(CDefiler* pOwner)
{
	m_IdleElapsedTime = 0;
	IdleDuration = 1.f;
}
