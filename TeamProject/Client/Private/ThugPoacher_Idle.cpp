#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Idle::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CThugPoacher_Idle::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Idle::Exit(CThugPoacher* pOwner)
{
}
