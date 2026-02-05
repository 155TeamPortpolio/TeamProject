#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Parried.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Parried::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Dodge")
		.Apply();
}

void CStrikeJaeger_Parried::Update(CStrikeJaeger* pOwner, _float dt)
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

void CStrikeJaeger_Parried::Exit(CStrikeJaeger* pOwner)
{
}
