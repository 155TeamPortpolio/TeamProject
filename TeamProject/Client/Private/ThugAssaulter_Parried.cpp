#include "pch.h"
#include "ThugAssaulter_Parried.h"
#include "ThugAssaulter.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Parried::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_H_Front")
		.Apply();
}

void CThugAssaulter_Parried::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CThugAssaulter_Parried::Exit(CThugAssaulter* pOwner)
{
}

