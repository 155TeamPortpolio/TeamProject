#include "pch.h"
#include "Claymore.h"
#include "Claymore_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Attack::Enter(CClaymore* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CClaymore>::Create();

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

void CClaymore_Attack::Exit(CClaymore* pOwner)
{
}

void CClaymore_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CClaymore_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CClaymore_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack02b", CClaymore_Attack2b::Create());
	m_pSubStateMachine->Register_State("Attack03", CClaymore_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack03_End", CClaymore_Attack3_End::Create());
	m_pSubStateMachine->Register_State("Attack04", CClaymore_Attack4::Create());
	m_pSubStateMachine->Register_State("Attack05", CClaymore_Attack5::Create());
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
	case 5:
		m_pSubStateMachine->Change_State("Attack05");
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
}

void CClaymore_Attack1::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Attack2::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Attack_02")
		.Apply();
}

void CClaymore_Attack2::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Attack2::Exit(CClaymore* pOwner)
{
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
}

void CClaymore_Attack3::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Attack3_End::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Attack_03_End")
		.Apply();
}

void CClaymore_Attack3_End::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Attack3_End::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Attack4::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Attack_04")
		.Apply();
}

void CClaymore_Attack4::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Attack4::Exit(CClaymore* pOwner)
{
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
