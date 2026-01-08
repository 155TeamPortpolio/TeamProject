#include "pch.h"
#include "JaneDoeState_Idle.h"

#include "JaneDoe.h"

#include "Animator3D.h"

void CJaneDoeState_Idle::Enter(CJaneDoe* pOwner)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("IdleEntryMode");
    pOwner->Get_StateMachine()->Set_Int("IdleEntryMode", 0);

    if (iEntryMode == 1)
    {
        pOwner->Get_StateMachine()->Set_Trigger("Evade");
    }

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Idle")
        .Loop(true)
        .Apply();
}