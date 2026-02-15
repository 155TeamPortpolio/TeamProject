#include "pch.h"
#include "DefilerState_Other.h"
#include "Defiler.h"
#include "StateMachine.h"
#include "BattleSystem.h"

void CDefilerState_Groggy::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	pOwner->ResetAllFlags();
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_IgnoreRotation();
	m_GroggyState = GroggyIn;
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Debuff_Stun_Start_Front")
		.Speed(1.f)
		.Loop(false)
		.Apply();
	pOwner->Release_AttackCollider();
}

void CDefilerState_Groggy::Update(CDefiler* pOwner, _float dt)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	switch (m_GroggyState)
	{
	case Client::CDefilerState_Groggy::GroggyIn:
		if (m_fAnimProgress > 0.95) {
			m_GroggyState = GroggyLoop;
			pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Debuff_Stun_Loop")
				.Speed(1.f)
				.Loop(true)
				.Apply();
		}
		break;
	case Client::CDefilerState_Groggy::GroggyLoop:
		if (pOwner->GetStatus().isGroggy == false) {
			m_GroggyState = GroggyLoop;
			pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Debuff_Stun_End")
				.Speed(1.f)
				.Loop(true)
				.Apply();
			m_GroggyState = GroggyEnd;
		}
		break;
	case Client::CDefilerState_Groggy::GroggyEnd:
		if (m_fAnimProgress > 0.95) {
			pOwner->Get_MainStateMachine()->Set_Trigger("Idle");
		}
		break;
	default:
		break;
	}
}

void CDefilerState_Groggy::Exit(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.LockRotate = false;
}

void CDefilerState_Death::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, pOwner->Get_Handle());
	pOwner->ResetAllFlags();
	pOwner->Release_AttackCollider();
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.ResetTraceFlag();
	blackBoard.TraceType_IgnoreRotation();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Death_Front")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Death::Update(CDefiler* pOwner, _float dt)
{
//	if (m_fAnimProgress > 0.9)
//		m_pOwnerStateMachine->Change_State();
}

void CDefilerState_Death::Exit(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Death_Stay")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}
