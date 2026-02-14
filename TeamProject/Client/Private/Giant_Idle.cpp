#include "pch.h"
#include "Claymore.h"
#include "Claymore_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Idle::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CClaymore_Idle::Update(CGiant* pOwner, _float dt)
{
}

void CClaymore_Idle::Exit(CGiant* pOwner)
{
}
