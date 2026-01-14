#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Born.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Born::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Born")
		.Apply();
	
}

void CThugAssaulter_Born::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Born::Exit(CThugAssaulter* pOwner)
{
}
