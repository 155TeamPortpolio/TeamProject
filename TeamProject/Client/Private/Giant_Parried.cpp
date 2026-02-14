#include "pch.h"
#include "Giant.h"
#include "Giant_Parried.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Parried::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Dodge")
		.Apply();
}

void CGiant_Parried::Update(CGiant* pOwner, _float dt)
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

void CGiant_Parried::Exit(CGiant* pOwner)
{
}
