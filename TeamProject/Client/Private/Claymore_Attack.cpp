#include "pch.h"
#include "Claymore.h"
#include "Claymore_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "AudioSource.h"

void CClaymore_Attack::Enter(CClaymore* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CClaymore>::Create();

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
		// 돌진 공격 빼고
		if (targetinginfo.fDistance <= hysteriesis.fComboExit)
			while (iAttackPatternIndex == 3 || iAttackPatternIndex == 0)
				iAttackPatternIndex = Helper::Get_Random_Int(1, 4);
		else  // 멀 때, 돌진공격  
			iAttackPatternIndex = 3;
	
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

}

void CClaymore_Attack::Update(CClaymore* pOwner, _float dt)
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
				pOwner->SetBattleColliderObject("Weapon", CEnemy::BATTLE_COLTYPE::ATTACK, true, m_HitDesc);
			else if (Event.Tag == "TurnOffAttackCol")
				pOwner->SetBattleColliderObject("Weapon", CEnemy::BATTLE_COLTYPE::ATTACK, false);
			else if (Event.Tag == "FinishAll")
				pOwner->SetOnAttack(false);
			else if (Event.Tag == "ParryEnable_false")
				pOwner->SetParryEnable(false);
			else if (Event.Tag == "ScrapingEnd")
				pOwner->Get_Component<CAudioSource>()->Set_SlotStop("claymore_Scraping.wav");

			
			break;
		}
		case Engine::CLIP_EVENT_TYPE::EFFECT:
			break;
		case Engine::CLIP_EVENT_TYPE::SOUND:
			if (Event.Tag == "claymore_Scraping.wav")
				pOwner->Get_Component<CAudioSource>()->Slot(Event.Tag).Attribute3D(true).Loop(2).Volume(0.05f).Play();
			else
				pOwner->Get_Component<CAudioSource>()->Slot(Event.Tag).Attribute3D(true).Play();

			break;
		default:
			break;
		}
	}

	if (m_fAnimProgress >= 0.99f)
		pOwner->Idle();
}

void CClaymore_Attack::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CClaymore_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CClaymore_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CClaymore_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack03_End", CClaymore_Attack3_End::Create());
	m_pSubStateMachine->Register_State("Attack04", CClaymore_Attack4::Create());
}

void CClaymore_Attack::Register_Transitions()
{
}

void CClaymore_Attack::AttackFromIndex(_int iMoveIndex)
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
void CClaymore_Attack1::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Attack_01")
		.Apply();
}

void CClaymore_Attack1::Update(CClaymore* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CClaymore_Attack1::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack1::Update_Effects(CClaymore* pOwner)
{
	if (IsCrossAnimProgress(0.2f))
		pOwner->Play_Effect("Claymore_Slash0_0", _vector3(0.f, 0.8f, 0.f), _quaternion(0.66f, -0.24f, 0.24f, 0.66f));
}

/*============================================================================*/
void CClaymore_Attack2::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Attack_02")
		.Apply();
}

void CClaymore_Attack2::Update(CClaymore* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CClaymore_Attack2::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack2::Update_Effects(CClaymore* pOwner)
{
	if (IsCrossAnimProgress(0.17f))
		pOwner->Play_Effect("Claymore_Slash0_0", _vector3(0.f, 1.4f, 0.f), _quaternion(-0.2f, 0.61f, 0.61f, -0.48f));
	if (IsCrossAnimProgress(0.3f))
		pOwner->Play_Effect("Claymore_Slash0_1", _vector3(0.f, 0.9f, 0.f), _quaternion(0.67f, -0.27f, 0.27f, 0.64f));
	if (IsCrossAnimProgress(0.44f))
		pOwner->Play_Effect("Claymore_Slash0_2", _vector3(0.2f, 1.f, 0.3f), _quaternion(0.01f, 0.77f, 0.01f, 0.63f));
}

/*============================================================================*/
void CClaymore_Attack2b::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Attack_02b")
		.Apply();
}

void CClaymore_Attack2b::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Attack2b::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Attack3::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Attack_03")
		.Apply();
}

void CClaymore_Attack3::Update(CClaymore* pOwner, _float dt)
{
	if (3.f >= pOwner->GetTargetingInfo().fDistance)
		m_pOwnerStateMachine->Change_State("Attack03_End");

	Update_Effects(pOwner);
}

void CClaymore_Attack3::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack3::Update_Effects(CClaymore* pOwner)
{
	if (IsCrossAnimProgress(0.42f))
		pOwner->Play_Effect("Claymore_Slash0_0", _vector3(0.f, 0.9f, 0.f), _quaternion(-0.06f, -0.64f, -0.51f, 0.57f));
	if (IsCrossAnimProgress(0.53f))
		pOwner->Play_Effect("Claymore_Slash0_1", _vector3(0.2f, 0.6f, 0.f), _quaternion(0.f, 0.72f, -0.05f, 0.69f));
}

/*============================================================================*/
void CClaymore_Attack3_End::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Attack_03_End")
		.Apply();
}

void CClaymore_Attack3_End::Update(CClaymore* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CClaymore_Attack3_End::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack3_End::Update_Effects(CClaymore* pOwner)
{
	if (IsCrossAnimProgress(0.05f))
		pOwner->Play_Effect("Claymore_Slash0_0", _vector3(0.f, 0.9f, 0.f), _quaternion(-0.06f, -0.64f, -0.51f, 0.57f));
	if (IsCrossAnimProgress(0.21f))
		pOwner->Play_Effect("Claymore_Slash0_1", _vector3(0.2f, 0.6f, 0.f), _quaternion(0.f, 0.72f, -0.05f, 0.69f));
}

/*============================================================================*/
void CClaymore_Attack4::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Attack_04")
		.Apply();
}

void CClaymore_Attack4::Update(CClaymore* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CClaymore_Attack4::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack4::Update_Effects(CClaymore* pOwner)
{
	if (IsCrossAnimProgress(0.2f))
		pOwner->Play_Effect("Claymore_Slash0_0", _vector3(0.f, 1.3f, 0.f), _quaternion(0.3f, 0.67f, 0.67f, 0.13f));
}

/*============================================================================*/
void CClaymore_Attack5::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Attack_05")
		.Apply();
}

void CClaymore_Attack5::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Attack5::Exit(CClaymore* pOwner)
{
}
