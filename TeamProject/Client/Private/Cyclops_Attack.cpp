#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Attack.h"
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
			_int iAttackPatternIndex = Helper::Get_Random_Int(1, 5);
			
		}
		AttackFromIndex(iAttackPatternIndex);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

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


	for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
	{
		switch (Event.Type)
		{
		case Engine::CLIP_EVENT_TYPE::NOTIFY:
		{
			if (Event.Tag == "UnleashAttack")
				pOwner->UnleashAttack(CEnemy::ATTACK_SIDE::NONE, false);
		/*	else if (Event.Tag == "Attack")
			{
			}*/
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
