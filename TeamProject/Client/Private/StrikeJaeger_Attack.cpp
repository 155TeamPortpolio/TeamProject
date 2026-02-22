#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Attack::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();

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

		if (targetinginfo.fDistance <= hysteriesis.fComboEnter)		// 완전 가까울때
			iAttackPatternIndex = 3;
		else if (targetinginfo.fDistance < hysteriesis.fComboExit)		// 적당히 전진 공격
			iAttackPatternIndex = 1;
		else if (targetinginfo.fDistance < hysteriesis.fChaseExit)		// 멀 때, 돌진공격
			iAttackPatternIndex = 2;
		else
		{
			// 너무멀면 다음행동
			pOwner->Idle();
			return;
		}
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

}

void CStrikeJaeger_Attack::Update(CStrikeJaeger* pOwner, _float dt)
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
				pOwner->SetBattleColliderObject("Weapon_L", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HitDesc);
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::LEFT);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOffAttackCol_L")
				pOwner->SetBattleColliderObject("Weapon_L", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			else if (Event.Tag == "TurnOnAttackCol_R")
			{
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HitDesc);
				pOwner->SetOnAttack(true, CEnemy::ATTACK_SIDE::RIGHT);
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
			}
			else if (Event.Tag == "TurnOffAttackCol_R")
				pOwner->SetBattleColliderObject("Weapon_R", CEnemy::BATTLE_COLTYPE::ATTACK, false);
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

void CStrikeJaeger_Attack::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CStrikeJaeger_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack03", CStrikeJaeger_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack05", CStrikeJaeger_Attack5::Create());
}

void CStrikeJaeger_Attack::Register_Transitions()
{
}

void CStrikeJaeger_Attack::AttackFromIndex(_int iMoveIndex)
{
	switch (iMoveIndex)
	{
	case 1:
		m_pSubStateMachine->Change_State("Attack01");
		break;
	case 2:
		m_pSubStateMachine->Change_State("Attack03");
		break;
	case 3:
		m_pSubStateMachine->Change_State("Attack05");
		break;
	}
}

/*============================================================================*/
void CStrikeJaeger_Attack1::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Attack01_Fast")
		.Apply();

}

void CStrikeJaeger_Attack1::Update(CStrikeJaeger* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CStrikeJaeger_Attack1::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Attack1::Update_Effects(CStrikeJaeger* pOwner)
{
	if (IsCrossAnimProgress(0.3f))
		pOwner->Play_Effect("Strike_Slash0_0", _vector3(0.f, 0.9f, 0.f), _quaternion(0.7f, 0.11f, 0.11f, 0.7f));
	if (IsCrossAnimProgress(0.48f))
		pOwner->Play_Effect("Strike_Slash0_1", _vector3(0.f, 1.f, 0.f), _quaternion(-0.08f, 0.68f, 0.69f, -0.24f));
}

/*============================================================================*/
void CStrikeJaeger_Attack3::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Attack03A_Fast")
		.Apply();
}

void CStrikeJaeger_Attack3::Update(CStrikeJaeger* pOwner, _float dt)
{
	if (3.f >= pOwner->GetTargetingInfo().fDistance)
		m_pOwnerStateMachine->Change_State("Attack03_End");

	Update_Effects(pOwner);
}

void CStrikeJaeger_Attack3::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Attack3::Update_Effects(CStrikeJaeger* pOwner)
{
	if (IsCrossAnimProgress(0.26f))
		pOwner->Play_Effect("Strike_Slash0_0", _vector3(0.f, 1.1f, 0.3f), _quaternion(0.67f, -0.16f, -0.13f, 0.71f));
	if (IsCrossAnimProgress(0.31f))
		pOwner->Play_Effect("Strike_Slash0_1", _vector3(0.f, 1.1f, 0.3f), _quaternion(0.35f, 0.63f, 0.68f, 0.13f));
	if (IsCrossAnimProgress(0.34f))
		pOwner->Play_Effect("Strike_Slash0_2", _vector3(0.f, 0.7f, 0.f), _quaternion(0.02f, 0.71f, 0.7f, 0.f));
	if (IsCrossAnimProgress(0.41f))
		pOwner->Play_Effect("Strike_Slash0_3", _vector3(0.f, 0.7f, 0.f), _quaternion(0.72f, -0.01f, -0.05f, 0.69f));
}

/*============================================================================*/
void CStrikeJaeger_Attack5::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Attack05_Fast")
		.Apply();
}

void CStrikeJaeger_Attack5::Update(CStrikeJaeger* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CStrikeJaeger_Attack5::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Attack5::Update_Effects(CStrikeJaeger* pOwner)
{
	if (IsCrossAnimProgress(0.17f))
		pOwner->Play_Effect("Strike_Slash0_0", _vector3(0.f, 1.f, 0.3f), _quaternion(0.66f, -0.28f, -0.25f, 0.65f));
	if (IsCrossAnimProgress(0.25f))
		pOwner->Play_Effect("Strike_Slash0_1", _vector3(0.f, 1.f, 0.3f), _quaternion(-0.39f, 0.56f, 0.69f, 0.27f));
	if (IsCrossAnimProgress(0.49f)) 
	{
		pOwner->Play_Effect("Strike_Slash0_2", _vector3(-0.3f, 1.1f, 0.3f), _quaternion(-0.25f, 0.67f, 0.65f, -0.27f));
		pOwner->Play_Effect("Strike_Slash0_3", _vector3(0.3f, 1.1f, 0.3f), _quaternion(-0.08f, 0.72f, 0.5f, 0.47f));

	}
}
