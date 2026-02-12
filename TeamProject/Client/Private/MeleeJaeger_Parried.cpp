#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Parried.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Parried::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_H_Front")
		.Apply();
}

void CMeleeJaeger_Parried::Update(CMeleeJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CMeleeJaeger_Parried::Exit(CMeleeJaeger* pOwner)
{
}
