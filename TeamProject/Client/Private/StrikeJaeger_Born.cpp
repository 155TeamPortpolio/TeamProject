#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Born::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Born")
		.Apply();
}

void CStrikeJaeger_Born::Update(CStrikeJaeger* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		0);
}

void CStrikeJaeger_Born::Exit(CStrikeJaeger* pOwner)
{
}
