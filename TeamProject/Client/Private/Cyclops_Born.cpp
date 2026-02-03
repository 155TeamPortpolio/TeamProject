#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Born::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Born")
		.Apply();
}

void CCyclops_Born::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Born::Exit(CCyclops* pOwner)
{
}
