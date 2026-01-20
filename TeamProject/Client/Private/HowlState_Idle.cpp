#include "pch.h"
#include "HowlState_Idle.h"
#include "Howl.h"

void CHowlState_Idle::Enter(CHowl* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Idle01")
        .Loop(true)
        .Apply();
}

void CHowlState_Idle::Update(CHowl* pOwner, _float dt)
{
}
