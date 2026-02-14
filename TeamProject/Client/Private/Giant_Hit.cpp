#include "pch.h"
#include "Giant.h"
#include "Giant_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Hit::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}


}

void CGiant_Hit::Update(CGiant* pOwner, _float dt)
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

void CGiant_Hit::Exit(CGiant* pOwner)
{
}

void CGiant_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Hit_H_Front", CGiant_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CGiant_Hit_H_Back::Create());
}

void CGiant_Hit::Register_Transitions()
{
}

/*============================================================================*/
void CGiant_Hit_H_Front::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Hit_H_Front")
		.Apply();
}

void CGiant_Hit_H_Front::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_H_Front::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_H_Back::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Hit_H_Back")
		.Apply();
}

void CGiant_Hit_H_Back::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_H_Back::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
