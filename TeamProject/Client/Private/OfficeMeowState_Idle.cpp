#include "pch.h"
#include "OfficeMeowState_Idle.h"

#include "OfficeMeow.h"

void COfficeMeowState_Idle::Enter(COfficeMeow* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle01")
        .Loop(true)
        .Apply();
}
