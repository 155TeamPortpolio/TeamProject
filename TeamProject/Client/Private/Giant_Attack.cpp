#include "pch.h"
#include "Giant.h"
#include "Giant_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Attack::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);

		m_NormalHitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
		m_NormalHitDesc.eHitType = HIT_TYPE::ONCE;
		m_NormalHitDesc.fDamage = 10.f;

		m_HardHitDesc.eDamageType = DAMAGE_TYPE::HARD;
		m_HardHitDesc.eHitType = HIT_TYPE::ONCE;
		m_HardHitDesc.fDamage = 15.f;

		m_KnockOutHitDesc.eDamageType = DAMAGE_TYPE::KNOCKOUT;
		m_KnockOutHitDesc.eHitType = HIT_TYPE::ONCE;
		m_KnockOutHitDesc.fDamage = 15.f;

		m_Attack3HitDesc.eDamageType = DAMAGE_TYPE::HARD;
		m_Attack3HitDesc.eHitType = HIT_TYPE::INTERVAL;
		m_Attack3HitDesc.fDamage = 10.f;
		m_Attack3HitDesc.fInterval = 0.5f;

		pOwner->AddAttackHistoryFront(0);
	}

	DecideAttackPattern(pOwner);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	if (true == blackboard.stateQueue.empty()) 
	{
		pOwner->Idle();
		return;
	}
	blackboard.isRequestNext = true;


}

void CGiant_Attack::Update(CGiant* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);


	for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
	{
		switch (Event.Type)
		{
		case Engine::CLIP_EVENT_TYPE::NOTIFY:
		{
			if (Event.Tag == "UnleashAttack")
				pOwner->UnleashAttack(CEnemy::ATTACK_SIDE::NONE, true);
			else if (Event.Tag == "TurnOnAttackCol_L")
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::LEFT);
				pOwner->SetBattleColliderObject("Weapon_L", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_NormalHitDesc);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOnAttackCol_L_Hard")
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::LEFT);
				pOwner->SetBattleColliderObject("Weapon_L", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HardHitDesc);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOnAttackCol_L_KnockOut")
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::LEFT);
				pOwner->SetBattleColliderObject("Weapon_L", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_KnockOutHitDesc);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOffAttackCol_L")
				pOwner->SetBattleColliderObject("Weapon_L", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			else if (Event.Tag == "TurnOnAttackCol_R")
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::RIGHT);
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_NormalHitDesc);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOnAttackCol_R_Hard")
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::RIGHT);
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HardHitDesc);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOnAttackCol_R_KnockOut")
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::RIGHT);
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_KnockOutHitDesc);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOffAttackCol_R")
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			else if (Event.Tag == "StartAttack3")		// 돌진 공격       
			{
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::RIGHT);
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_Attack3HitDesc);
			}
			else if (Event.Tag == "FinishAll")
				pOwner->SetOnAttack(false);
			else if (Event.Tag == "ParryDisable")
				pOwner->SetParryEnable(false);

			
			break;
		}
		case Engine::CLIP_EVENT_TYPE::EFFECT:
			break;
		case Engine::CLIP_EVENT_TYPE::SOUND:
			break;
		default:
			break;
		}
	}

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.99f)
	{
		blackboard.isChainOpen = true;

		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
		else
		{
			_int iComboAttackCount = pOwner->GetAttackCombo();
			if (iComboAttackCount >= 2)
			{
				pOwner->ResetAttackCombo();
				blackboard.isRequestNext = false;
			}
			else
			{
				DecideAttackPattern(pOwner);
				if (!blackboard.stateQueue.empty())
					blackboard.isRequestNext = true;
			}
		}
	}

	if (true == blackboard.isRequestNext) {
		blackboard.isRequestNext = false;
		blackboard.isChainOpen = false;

		if (!blackboard.stateQueue.empty()) {
			string nextStateTag = blackboard.stateQueue.front();
			blackboard.stateQueue.pop_front();

			blackboard.currentStateTag = nextStateTag;
			m_pSubStateMachine->Change_State(nextStateTag);
		}
	}

	if (true == blackboard.isChainOpen && false == blackboard.isRequestNext) {
		blackboard.currentStateTag = "";
		pOwner->Idle();
	}
}

void CGiant_Attack::Exit(CGiant* pOwner)
{
}

void CGiant_Attack::DecideAttackPattern(CGiant* pOwner)
{
	auto pStateMachine = pOwner->GetStateMachine();
	if (nullptr == pStateMachine)
	{
		pOwner->Idle();
		return;
	}

	auto hysteriesis = pOwner->GetHysteriesis();
	auto targetinginfo = pOwner->GetTargetingInfo();

	_int iAttackPatternIndex = pStateMachine->Get_Int("AttackPattern");
	if (0 != iAttackPatternIndex)
		pStateMachine->Set_Int("AttackPattern", 0);
	else {
		if (targetinginfo.fDistance <= hysteriesis.fComboEnter)				// 근점
		{
			iAttackPatternIndex = pOwner->GetAttackHistoryFront();
			while (iAttackPatternIndex == pOwner->GetAttackHistoryFront())
			{
				_int i = Helper::Get_Random_Int(1, 2);
				if (i == 1)
					iAttackPatternIndex = ATTACK::Attack1;
				else
					iAttackPatternIndex = ATTACK::Attack2;
			}
		}
		else if (targetinginfo.fDistance <= hysteriesis.fChaseExit)			// 중거리 공격(도약 공격 등)
		{
			iAttackPatternIndex = pOwner->GetAttackHistoryFront();
			while (iAttackPatternIndex == pOwner->GetAttackHistoryFront())
			{
				_int i = Helper::Get_Random_Int(1, 4);
				switch (i)
				{
				case 1:
					iAttackPatternIndex = ATTACK::Attack2_Explode;
					break;
				case 2:
					iAttackPatternIndex = ATTACK::Attack3;
					break;
				case 3:
					iAttackPatternIndex = ATTACK::Attack4;
					break;
				case 4:
					iAttackPatternIndex = ATTACK::Attack5;
					break;
				}
			}
		}
		else if (targetinginfo.fDistance <= hysteriesis.fComboExit)			// 장거리 공격(도약 및 돌진)
		{
			iAttackPatternIndex = pOwner->GetAttackHistoryFront();
			while (iAttackPatternIndex == pOwner->GetAttackHistoryFront())
			{
				_int i = Helper::Get_Random_Int(1, 2);
				switch (i)
				{
				case 1:
					iAttackPatternIndex = ATTACK::Attack2_1;
					break;
				case 2:
					iAttackPatternIndex = ATTACK::Attack3;
					break;
				}
			}
		}
	}
	AttackFromIndex(pOwner, iAttackPatternIndex);
}

void CGiant_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack1", CGiant_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack2", CGiant_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack2_1", CGiant_Attack2_1::Create());
	m_pSubStateMachine->Register_State("Attack2_Explode", CGiant_Attack2_Explode::Create());
	m_pSubStateMachine->Register_State("Attack3", CGiant_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack3_HitWall", CGiant_Attack3_HitWall::Create());
	m_pSubStateMachine->Register_State("Attack4", CGiant_Attack4::Create());
	m_pSubStateMachine->Register_State("Attack5", CGiant_Attack5::Create());
}

void CGiant_Attack::Register_Transitions()
{
}

void CGiant_Attack::AttackFromIndex(CGiant* pOwner, _int iMoveIndex)
{
	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();

	switch (iMoveIndex)
	{
	case Client::CGiant_Attack::Attack1:
		blackboard.stateQueue.push_back("Attack1");
		break;
	case Client::CGiant_Attack::Attack2:
		blackboard.stateQueue.push_back("Attack2");
		break;
	case Client::CGiant_Attack::Attack2_1:
		blackboard.stateQueue.push_back("Attack2_1");
		break;
	case Client::CGiant_Attack::Attack2_Explode:
		blackboard.stateQueue.push_back("Attack2_Explode");
		break;
	case Client::CGiant_Attack::Attack3:
		blackboard.stateQueue.push_back("Attack3");
		break;
	case Client::CGiant_Attack::Attack3_HitWall:
		blackboard.stateQueue.push_back("Attack3_HitWall");
		break;
	case Client::CGiant_Attack::Attack4:
		blackboard.stateQueue.push_back("Attack4");
		break;
	case Client::CGiant_Attack::Attack5:
		blackboard.stateQueue.push_back("Attack5");
		break;
	default:
		return;
	}

	pOwner->AddAttackHistoryFront(iMoveIndex);
	pOwner->AddAttackCombo();
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
}

/*============================================================================*/
void CGiant_Attack1::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_01")
		.Apply();
}

void CGiant_Attack1::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack1::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack2::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_02")
		.Apply();

	pOwner->SetParryDontStop(true);
}

void CGiant_Attack2::Update(CGiant* pOwner, _float dt)
{
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
		if (Event.Type == CLIP_EVENT_TYPE::NOTIFY)
			if (Event.Tag == "Skip")
			{
				auto TargetingInfo = pOwner->GetTargetingInfo();
				auto Hysteriesis = pOwner->GetHysteriesis();
				if (Hysteriesis.fComboEnter <= TargetingInfo.fDistance &&
					Hysteriesis.fComboExit >= TargetingInfo.fDistance)
					m_pOwnerStateMachine->Change_State("Attack2_Explode");
			}

}

void CGiant_Attack2::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack2_1::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_02_01")
		.Apply();

	pOwner->SetParryDontStop(true);
}

void CGiant_Attack2_1::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack2_1::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack2_Explode::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_02_Explode")
		.Apply();

	pOwner->SetParryDontStop(true);
}

void CGiant_Attack2_Explode::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack2_Explode::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack3::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_03")
		.Apply();
}

void CGiant_Attack3::Update(CGiant* pOwner, _float dt)
{
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
}

void CGiant_Attack3::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack3_HitWall::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_03_Hitwall")
		.Apply();
}

void CGiant_Attack3_HitWall::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack3_HitWall::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack4::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_04")
		.Apply();
}

void CGiant_Attack4::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack4::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack5::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_05")
		.Apply();

	pOwner->SetParryDontStop(true);
}

void CGiant_Attack5::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack5::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack6_AttackBack::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_06_AttackBack")
		.Apply();
}

void CGiant_Attack6_AttackBack::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack6_AttackBack::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack7::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_07")
		.Apply();
}

void CGiant_Attack7::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack7::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack7_Jump::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_07_Jump")
		.Apply();
}

void CGiant_Attack7_Jump::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack7_Jump::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack7_Revenge::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_07_Revenge")
		.Apply();
}

void CGiant_Attack7_Revenge::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack7_Revenge::Exit(CGiant* pOwner)
{
}
