#include "pch.h"
#include "Giant.h"
#include "Giant_Parried.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Parried::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	CEnemy::ATTACK_SIDE eSide = pOwner->Get_AttackSide();

	switch (eSide)
	{
	case Client::CEnemy::ATTACK_SIDE::LEFT:
		m_pSubStateMachine->Change_State("Parried_Left");
		break;
	case Client::CEnemy::ATTACK_SIDE::RIGHT:
		m_pSubStateMachine->Change_State("Parried_Right");
		break;
	}
}

void CGiant_Parried::Update(CGiant* pOwner, _float dt)
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

void CGiant_Parried::Exit(CGiant* pOwner)
{
}

void CGiant_Parried::Register_States()
{
	m_pSubStateMachine->Register_State("Parried_Left", CGiant_Parried_Left::Create());
	m_pSubStateMachine->Register_State("Parried_Right", CGiant_Parried_Right::Create());
}

void CGiant_Parried::Register_Transitions()
{
}

/*============================================================================*/
void CGiant_Parried_Left::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Hit_Parry_Left")
		.Apply();
}

void CGiant_Parried_Left::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Parried_Left::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Parried_Right::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Hit_Parry_Right")
		.Apply();
}

void CGiant_Parried_Right::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Parried_Right::Exit(CGiant* pOwner)
{
}
