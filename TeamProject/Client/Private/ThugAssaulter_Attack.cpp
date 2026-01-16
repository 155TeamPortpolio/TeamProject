#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Attack::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}

	auto pStateMachine = pOwner->GetStateMachine();
	if (nullptr == pStateMachine)
		return;

	_int iAttackPatternIndex = pStateMachine->Get_Int("AttackPattern");
	if (0 != iAttackPatternIndex) {
		pStateMachine->Set_Int("AttackPattern", 0);
		AttackFromIndex(iAttackPatternIndex);
	}
	else {
		iAttackPatternIndex = Helper::Get_Random_Int(1, 4);
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
}

void CThugAssaulter_Attack::Update(CThugAssaulter* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if (m_fAnimProgress >= 0.99f) 
		pOwner->Idle();
	
}

void CThugAssaulter_Attack::Exit(CThugAssaulter* pOwner)
{
}

void CThugAssaulter_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CThugAssaulter_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CThugAssaulter_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CThugAssaulter_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack04", CThugAssaulter_Attack4::Create());
}

void CThugAssaulter_Attack::Register_Transitions()
{
}

void CThugAssaulter_Attack::AttackFromIndex(_int iMoveIndex)
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
void CThugAssaulter_Attack1::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Attack_01")
		.Apply();
	pOwner->Active_AttackSign();

	HitDesc hitdesc = {};
	hitdesc.eDamageType = DAMAGE_TYPE::NORMAL;
	hitdesc.eHitType = HIT_TYPE::ONCE;
	hitdesc.fDamage = 10.f;

	pOwner->SetAutoPlayBattleCollider("Weapon", 0.17f, 0.02f, hitdesc);
}

void CThugAssaulter_Attack1::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugAssaulter_Attack1::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Attack2::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Attack_02")
		.Apply();
	pOwner->Active_AttackSign();
	
	m_tHitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
	m_tHitDesc.eHitType = HIT_TYPE::ONCE;
	m_tHitDesc.fDamage = 10.f;

	pOwner->SetAutoPlayBattleCollider("Weapon", 0.2f, 0.03f, m_tHitDesc);
	m_isFirstAttack = false;
	m_isSecondAttack = false;
}

void CThugAssaulter_Attack2::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	if (false == m_isFirstAttack &&
		m_fAnimProgress > 0.24) {
		pOwner->SetAutoPlayBattleCollider("Weapon", 0.31f, 0.03f, m_tHitDesc);
		m_isFirstAttack = true;
	}

	if (true == m_isFirstAttack &&
		false == m_isSecondAttack &&
		m_fAnimProgress > 0.24) {
		pOwner->SetAutoPlayBattleCollider("Weapon", 0.47f, 0.02f, m_tHitDesc);
		m_isSecondAttack = true;
	}


}

void CThugAssaulter_Attack2::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Attack3::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Attack_03")
		.Apply();
	pOwner->Active_AttackSign();

	m_tHitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
	m_tHitDesc.eHitType = HIT_TYPE::ONCE;
	m_tHitDesc.fDamage = 10.f;

	pOwner->SetAutoPlayBattleCollider("Weapon", 0.25f, 0.05f, m_tHitDesc);
	m_isFirstAttack = false;
}

void CThugAssaulter_Attack3::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	if (false == m_isFirstAttack &&
		m_fAnimProgress > 0.35) {
		pOwner->SetAutoPlayBattleCollider("Weapon", 0.42f, 0.03f, m_tHitDesc);
		m_isFirstAttack = true;
	}
}

void CThugAssaulter_Attack3::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Attack4::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Attack_04")
		.Apply();
	pOwner->Active_AttackSign();

	HitDesc hitdesc = {};
	hitdesc.eDamageType = DAMAGE_TYPE::NORMAL;
	hitdesc.eHitType = HIT_TYPE::ONCE;
	hitdesc.fDamage = 10.f;

	pOwner->SetAutoPlayBattleCollider("Weapon", 0.23f, 0.35f, hitdesc);
}

void CThugAssaulter_Attack4::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugAssaulter_Attack4::Exit(CThugAssaulter* pOwner)
{
}
