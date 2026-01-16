#include "pch.h"
#include "BelleState_Idle.h"
#include "Belle.h"

void CBelleState_Idle::Enter(CBelle* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Ani_MainCity_Idle")
        .Loop(true)
        .Apply();
}
