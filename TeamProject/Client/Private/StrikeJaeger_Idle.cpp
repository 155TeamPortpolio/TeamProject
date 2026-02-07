#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Idle.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Idle::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CStrikeJaeger_Idle::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Idle::Exit(CStrikeJaeger* pOwner)
{
}
