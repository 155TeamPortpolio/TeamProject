#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Attack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "ThugAssaulter_Idle.h"

void CThugAssaulter_Idle::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CThugAssaulter_Idle::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Idle::Exit(CThugAssaulter* pOwner)
{
}
