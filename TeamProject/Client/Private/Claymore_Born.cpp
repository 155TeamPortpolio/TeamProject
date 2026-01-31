#include "pch.h"
#include "Claymore.h"
#include "Claymore_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Born::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Born")
		.Apply();
}

void CClaymore_Born::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Born::Exit(CClaymore* pOwner)
{
}
