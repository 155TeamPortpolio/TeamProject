#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"


void CThugPoacher_Attack::Enter(CThugPoacher* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugPoacher>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
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
		//iAttackPatternIndex = Helper::Get_Random_Int(1, 3);
		
		if (targetinginfo.fDistance <= hysteriesis.fComboEnter)
			iAttackPatternIndex = 2;		// 뒷 구르기 후 한 발 공격
		else
		{
			_int i = Helper::Get_Random_Int(0, 2);
			if (i == 0)
				iAttackPatternIndex = 3;	// 한 발 공격
			else
				iAttackPatternIndex = 1;	// 세 발 공격
		}
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
}

void CThugPoacher_Attack::Update(CThugPoacher* pOwner, _float dt)
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
				pOwner->UnleashAttack(CEnemy::ATTACK_SIDE::NONE, false);
			else if (Event.Tag == "Shoot")
			{
				pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);
				pOwner->ShootArrow();
			}
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

void CThugPoacher_Attack::Exit(CThugPoacher* pOwner)
{
}

void CThugPoacher_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CThugPoacher_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CThugPoacher_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CThugPoacher_Attack3::Create());
}

void CThugPoacher_Attack::Register_Transitions()
{
}

void CThugPoacher_Attack::AttackFromIndex(_int iMoveIndex)
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
	}
}

/*============================================================================*/
void CThugPoacher_Attack1::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Attack_01")
		.Apply();
}

void CThugPoacher_Attack1::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Attack1::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Attack2::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Attack_02")
		.Apply();
}

void CThugPoacher_Attack2::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Attack2::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Attack3::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Attack_03")
		.Apply();
}

void CThugPoacher_Attack3::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Attack3::Exit(CThugPoacher* pOwner)
{
}
