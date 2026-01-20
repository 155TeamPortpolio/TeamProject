#include "pch.h"
#include "HowlState_Sleep.h"
#include "Howl.h"

void CHowlState_Sleep::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle04")
        .Loop(true)
        .Apply();
}
