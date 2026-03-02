#include "pch.h"
#include "Giant.h"
#include "Giant_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Idle::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Giant_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CGiant_Idle::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Idle::Exit(CGiant* pOwner)
{
}
