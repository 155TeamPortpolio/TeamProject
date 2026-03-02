#include "pch.h"
#include "Giant.h"
#include "Giant_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Born::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Born")
		.Apply();
}

void CGiant_Born::Update(CGiant* pOwner, _float dt)
{
	pOwner->Update_SpawnSequence(dt);
}

void CGiant_Born::Exit(CGiant* pOwner)
{
}
