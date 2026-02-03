#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Idle::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CCyclops_Idle::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Idle::Exit(CCyclops* pOwner)
{
}
