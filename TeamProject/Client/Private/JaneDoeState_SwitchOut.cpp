#include "pch.h"
#include "JaneDoeState_SwitchOut.h"

#include "JaneDoe.h"

void CJaneDoeState_SwitchOut::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchOut_Normal")
        .Loop(false)
        .Apply();
}
