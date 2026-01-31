#include "pch.h"
#include "Claymore.h"
#include "Claymore_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Idle::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CClaymore_Idle::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Idle::Exit(CClaymore* pOwner)
{
}
