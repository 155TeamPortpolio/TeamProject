#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Idle::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CMeleeJaeger_Idle::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Idle::Exit(CMeleeJaeger* pOwner)
{
}
