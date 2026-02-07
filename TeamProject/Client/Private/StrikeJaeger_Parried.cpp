#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Parried.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Parried::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}

	CEnemy::ATTACK_SIDE eSide = pOwner->Get_AttackSide();

	switch (eSide)
	{
	case Client::CEnemy::ATTACK_SIDE::NONE:
		m_pSubStateMachine->Change_State("Dodge");
		break;
	case Client::CEnemy::ATTACK_SIDE::LEFT:
		m_pSubStateMachine->Change_State("Dodge_L");
		break;
	case Client::CEnemy::ATTACK_SIDE::RIGHT:
		m_pSubStateMachine->Change_State("Dodge_R");
		break;
	}
}

void CStrikeJaeger_Parried::Update(CStrikeJaeger* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CStrikeJaeger_Parried::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Parried::Register_States()
{
	m_pSubStateMachine->Register_State("Dodge", CStrikeJaeger_Dodge::Create());
	m_pSubStateMachine->Register_State("Dodge_L", CStrikeJaeger_Dodge_L::Create());
	m_pSubStateMachine->Register_State("Dodge_R", CStrikeJaeger_Dodge_R::Create());
}

void CStrikeJaeger_Parried::Register_Transitions()
{
}

/*============================================================================*/
void CStrikeJaeger_Dodge::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Dodge")
		.Apply();
}

void CStrikeJaeger_Dodge::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Dodge::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Dodge_L::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Dodge_L")
		.Apply();
}

void CStrikeJaeger_Dodge_L::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Dodge_L::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Dodge_R::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Dodge_R")
		.Apply();
}

void CStrikeJaeger_Dodge_R::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Dodge_R::Exit(CStrikeJaeger* pOwner)
{
}
