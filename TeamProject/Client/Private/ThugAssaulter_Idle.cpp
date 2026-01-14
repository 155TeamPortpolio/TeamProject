#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "ThugAssaulter_Idle.h"

void CThugAssaulter_Idle::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_MainCity_Stand_Idle01")
		.Loop(true)
		.Apply();
}

void CThugAssaulter_Idle::Update(CThugAssaulter* pOwner, _float dt)
{
	//_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	//_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	//pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
	//	vRootBoneMoveDelta,
	//	qRot,
	//	dt);

}

void CThugAssaulter_Idle::Exit(CThugAssaulter* pOwner)
{
}
