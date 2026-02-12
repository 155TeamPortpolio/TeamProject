#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Attack::Enter(CMeleeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CMeleeJaeger>::Create();

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

		if (pOwner->IsShield())
		{
			if (targetinginfo.fDistance <= hysteriesis.fEvadeEnter)		// 완전 가까울때
				iAttackPatternIndex = 5;
			else if (targetinginfo.fDistance < hysteriesis.fComboExit)		// 적당히 전진 공격
				iAttackPatternIndex = 2;
			else
			{
				// 너무멀면 다음행동
				pOwner->Idle();
				return;
			}
		}
		else
		{
			if (targetinginfo.fDistance <= hysteriesis.fComboEnter)		// 완전 가까울때
			{
				_int i = Helper::Get_Random_Int(1, 3);
				switch (i)
				{
				case 1:
					iAttackPatternIndex = 2;
					break;
				case 2:
					iAttackPatternIndex = 3;
					break;
				case 3:
					iAttackPatternIndex = 5;
					break;
				}
			}
			else if (targetinginfo.fDistance < hysteriesis.fComboExit)		// 적당히 전진 공격
				iAttackPatternIndex = 4;
			else if (targetinginfo.fDistance < hysteriesis.fChaseExit)		// 적당히 전진 공격
				iAttackPatternIndex = 1;
			else
			{
				// 너무멀면 다음행동
				pOwner->Idle();
				return;
			}
		}
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

}

void CMeleeJaeger_Attack::Update(CMeleeJaeger* pOwner, _float dt)
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
			else if (Event.Tag == "TurnOnAttackCol")
			{
				pOwner->SetBattleColliderObject("Weapon", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HitDesc);
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::RIGHT);
			}
			else if (Event.Tag == "TurnOffAttackCol")
				pOwner->SetBattleColliderObject("Weapon", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			else if (Event.Tag == "FinishAll")
				pOwner->SetOnAttack(false);
			else if (Event.Tag == "ShieldRollStart")
				pOwner->StartRoll(-40.f);
			else if (Event.Tag == "ShieldRollEnd")
				pOwner->EndRoll();
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

void CMeleeJaeger_Attack::Exit(CMeleeJaeger* pOwner)
{
}

void CMeleeJaeger_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CMeleeJaeger_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CMeleeJaeger_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CMeleeJaeger_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack04", CMeleeJaeger_Attack4::Create());
	m_pSubStateMachine->Register_State("Attack05", CMeleeJaeger_Attack5::Create());
}

void CMeleeJaeger_Attack::Register_Transitions()
{
}

void CMeleeJaeger_Attack::AttackFromIndex(_int iMoveIndex)
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
	case 5:
		m_pSubStateMachine->Change_State("Attack05");
		break;
	}
}

/*============================================================================*/
void CMeleeJaeger_Attack1::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Attack_01")
		.Apply();
}

void CMeleeJaeger_Attack1::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Attack1::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Attack2::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Attack_02")
		.Apply();
}

void CMeleeJaeger_Attack2::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Attack2::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Attack3::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Attack_03")
		.Apply();
}

void CMeleeJaeger_Attack3::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Attack3::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Attack4::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Attack_04")
		.Apply();
}

void CMeleeJaeger_Attack4::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Attack4::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Attack5::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Attack_05")
		.Apply();
}

void CMeleeJaeger_Attack5::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Attack5::Exit(CMeleeJaeger* pOwner)
{
}
