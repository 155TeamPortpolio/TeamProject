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

		m_HitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
		m_HitDesc.eHitType = HIT_TYPE::ONCE;
		m_HitDesc.fDamage = 10.f;
	}

	auto pStateMachine = pOwner->GetStateMachine();
	if (nullptr == pStateMachine)
		return;

	auto hysteriesis = pOwner->GetHysteriesis();
	auto targetinginfo = pOwner->GetTargetingInfo();

	_int iAttackPatternIndex = pStateMachine->Get_Int("AttackPattern");
	if (0 != iAttackPatternIndex) 
		pStateMachine->Set_Int("AttackPattern", 0);
	else {
		//돌진 공격 빼고
		if (targetinginfo.fDistance <= hysteriesis.fComboEnter)
		{
			_int i = Helper::Get_Random_Int(1, 2);
			if (i == 1)
				iAttackPatternIndex = ATTACK::Attack1;
			else 
				iAttackPatternIndex = ATTACK::Attack2;
		}
		else if (targetinginfo.fDistance < hysteriesis.fLeapAttack)
		{
			_int i = Helper::Get_Random_Int(1, 2);
			if (i == 1)
				iAttackPatternIndex = ATTACK::Attack4;
			else
				iAttackPatternIndex = ATTACK::Attack5;
		}
		else if (targetinginfo.fDistance < hysteriesis.fJumpShort)
		{
			_int i = Helper::Get_Random_Int(1, 2);
			if (i == 1)
				iAttackPatternIndex = ATTACK::Attack2_Explode;
			else
				iAttackPatternIndex = ATTACK::Attack3;
		}
		else if (targetinginfo.fDistance < hysteriesis.fJumpLong)
			iAttackPatternIndex = ATTACK::Attack2_1;
		else
		{
			pOwner->Idle();
			return;
		}
	}
	AttackFromIndex(iAttackPatternIndex);
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

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
			//if (Event.Tag == "UnleashAttack")
			//	pOwner->UnleashAttack(CEnemy::ATTACK_SIDE::NONE, true);
			//else if (Event.Tag == "TurnOnAttackCol")
			//	pOwner->SetBattleColliderObject("Weapon", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HitDesc);
			//else if (Event.Tag == "TurnOffAttackCol")
			//	pOwner->SetBattleColliderObject("Weapon", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			//else if (Event.Tag == "FinishAll")
			//	pOwner->SetOnAttack(false);
			//break;
			//"StartAttack3"

		}
		case Engine::CLIP_EVENT_TYPE::EFFECT:
			break;
		case Engine::CLIP_EVENT_TYPE::SOUND:
			break;
		default:
			break;
		}
	}

	if (m_fAnimProgress >= 0.99f)
		pOwner->Idle();
}

void CGiant_Attack::Exit(CGiant* pOwner)
{
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
	//m_pSubStateMachine->Register_State("Attack6_AttackBack", CGiant_Attack6_AttackBack::Create());
	//m_pSubStateMachine->Register_State("Attack7", CGiant_Attack7::Create());
	//m_pSubStateMachine->Register_State("Attack7_Jump", CGiant_Attack7_Jump::Create());
	//m_pSubStateMachine->Register_State("Attack7_Revenge", CGiant_Attack7_Revenge::Create());
}

void CGiant_Attack::Register_Transitions()
{
}

void CGiant_Attack::AttackFromIndex(_int iMoveIndex)
{
	switch (iMoveIndex)
	{
	case Client::CGiant_Attack::Attack1:
		m_pSubStateMachine->Change_State("Attack1");
		break;
	case Client::CGiant_Attack::Attack2:
		m_pSubStateMachine->Change_State("Attack2");
		break;
	case Client::CGiant_Attack::Attack2_1:
		m_pSubStateMachine->Change_State("Attack2_1");
		break;
	case Client::CGiant_Attack::Attack2_Explode:
		m_pSubStateMachine->Change_State("Attack2_Explode");
		break;
	case Client::CGiant_Attack::Attack3:
		m_pSubStateMachine->Change_State("Attack3");
		break;
	case Client::CGiant_Attack::Attack3_HitWall:
		m_pSubStateMachine->Change_State("Attack3_HitWall");
		break;
	case Client::CGiant_Attack::Attack4:
		m_pSubStateMachine->Change_State("Attack4");
		break;
	case Client::CGiant_Attack::Attack5:
		m_pSubStateMachine->Change_State("Attack5");
		break;
	//case Client::CGiant_Attack::Attack6_AttackBack:
	//	m_pSubStateMachine->Change_State("Attack6_AttackBack");
	//	break;
	//case Client::CGiant_Attack::Attack7:
	//	m_pSubStateMachine->Change_State("Attack7");
	//	break;
	//case Client::CGiant_Attack::Attack7_Jump:
	//	m_pSubStateMachine->Change_State("Attack7_Jump");
	//	break;
	//case Client::CGiant_Attack::Attack7_Revenge:
	//	m_pSubStateMachine->Change_State("Attack7_Revenge");
	//	break;
	default:
		break;
	}
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
}

void CGiant_Attack2::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Attack2::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Attack2_1::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Attack_02_01")
		.Apply();
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
