#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Born::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Born")
		.Apply();
}

void CMeleeJaeger_Born::Update(CMeleeJaeger* pOwner, _float dt)
{
	pOwner->Update_SpawnSequence(dt);

	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		0);
}

void CMeleeJaeger_Born::Exit(CMeleeJaeger* pOwner)
{
}
