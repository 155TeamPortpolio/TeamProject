#include "pch.h"
#include "OfficeMeowState_Think.h"

#include "OfficeMeow.h"

void COfficeMeowState_Think::Enter(COfficeMeow* pOwner)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();
    pAnimator->Change_Animation(pOwner->Get_AnimName() + "Think01")
        .Loop(false)
        .Apply();
}
