#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Attack.h"
#include "Cyclops_Spit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Attack::Enter(CCyclops* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CCyclops>::Create();

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
	if (0 != iAttackPatternIndex) {
		pStateMachine->Set_Int("AttackPattern", 0);
		AttackFromIndex(iAttackPatternIndex);
	}
	else {
		if (targetinginfo.fDistance <= hysteriesis.fEvadeEnter) // 가까울때
		{
			_int i = Helper::Get_Random_Int(0, 3);
			if (0 == i)
				iAttackPatternIndex = 4;	// 25% 확률로 백스텝하면서 침뱉는 공격
			else
				iAttackPatternIndex = 3;	// 75% 확률로 머가리로 후림
		}
		else if (targetinginfo.fDistance < hysteriesis.fComboEnter)
			iAttackPatternIndex = 1;		// 침뱉음
		else if (targetinginfo.fDistance < hysteriesis.fChaseExit)
			iAttackPatternIndex = 2;		// 적당히 멀면 Shoot
		else
		{
			// 너무멀면 다음행동
			pOwner->Idle();
			return;
		}
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	m_isStopRotate = false;
}

void CCyclops_Attack::Update(CCyclops* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	if (false == m_isStopRotate)
		pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
	else
		int a = 1;


	for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
	{
		switch (Event.Type)
		{
		case Engine::CLIP_EVENT_TYPE::NOTIFY:
		{
			if (Event.Tag == "UnleashAttackParry_false")
				pOwner->UnleashAttack(CEnemy::ATTACK_SIDE::NONE, false);
			else if (Event.Tag == "UnleashAttackParry_true")
				pOwner->UnleashAttack(CEnemy::ATTACK_SIDE::NONE, true);
			else if (Event.Tag == "TurnOnAttackCol")
				pOwner->SetBattleColliderObject("Head", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HitDesc);
			else if (Event.Tag == "TurnOffAttackCol")
				pOwner->SetBattleColliderObject("Head", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			else if (Event.Tag == "Shoot")
			{
				pOwner->Spit(ENUM(CCyclops_Spit::SPIT::STRAIGHT));
				m_isStopRotate = true;
			}
			else if (Event.Tag == "SpitOnce")
			{
				pOwner->Spit(ENUM(CCyclops_Spit::SPIT::ARC_CENTER));
				m_isStopRotate = true;
			}
			else if (Event.Tag == "SpitThreeTimes1")
			{
				pOwner->Spit(ENUM(CCyclops_Spit::SPIT::ARC_CENTER));
				m_isStopRotate = true;
			}
			else if (Event.Tag == "SpitThreeTimes2")
				pOwner->Spit(ENUM(CCyclops_Spit::SPIT::ARC_LEFT));
			else if (Event.Tag == "SpitThreeTimes3")
				pOwner->Spit(ENUM(CCyclops_Spit::SPIT::ARC_RIGHT));
			else if (Event.Tag == "FinishAll")
				pOwner->SetOnAttack(false);


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

	if (m_fAnimProgress >= 0.99f)
		pOwner->Idle();
}

void CCyclops_Attack::Exit(CCyclops* pOwner)
{
}

void CCyclops_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CCyclops_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CCyclops_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CCyclops_Attack3_Hit::Create());
	m_pSubStateMachine->Register_State("Attack03_End", CCyclops_Attack3_End::Create());
	m_pSubStateMachine->Register_State("Attack04", CCyclops_Attack4::Create());
}

void CCyclops_Attack::Register_Transitions()
{
}

void CCyclops_Attack::AttackFromIndex(_int iMoveIndex)
{
	switch (iMoveIndex)
	{
	case 1:
		m_pSubStateMachine->Change_State("Attack01");
		break;
	case 2:
		m_pSubStateMachine->Change_State("Attack02");
		break;
	case 3:
		m_pSubStateMachine->Change_State("Attack03");
		break;
	case 4:
		m_pSubStateMachine->Change_State("Attack04");
		break;
	}
}

/*============================================================================*/
void CCyclops_Attack1::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Attack_01")
		.Apply();
}

void CCyclops_Attack1::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Attack1::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Attack2::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Attack_02")
		.Apply();
}

void CCyclops_Attack2::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Attack2::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Attack3_Hit::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Attack_03_01_Hit")
		.Apply();
}

void CCyclops_Attack3_Hit::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Attack3_Hit::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Attack3_End::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Attack_03_01_Miss")
		.Apply();
}

void CCyclops_Attack3_End::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Attack3_End::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Attack4::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Attack_04")
		.Apply();
}

void CCyclops_Attack4::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Attack4::Exit(CCyclops* pOwner)
{
}
