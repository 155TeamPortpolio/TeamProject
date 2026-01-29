#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Born::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Born")
		.Apply();
}

void CThugPoacher_Born::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Born::Exit(CThugPoacher* pOwner)
{
}
