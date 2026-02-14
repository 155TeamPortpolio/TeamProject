#include "pch.h"
#include "Claymore.h"
#include "Claymore_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Born::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Born")
		.Apply();
}

void CClaymore_Born::Update(CGiant* pOwner, _float dt)
{
}

void CClaymore_Born::Exit(CGiant* pOwner)
{
}
