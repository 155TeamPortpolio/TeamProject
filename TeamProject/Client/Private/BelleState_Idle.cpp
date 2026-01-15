#include "pch.h"
#include "BelleState_Idle.h"
#include "Belle.h"

void CBelleState_Idle::Enter(CBelle* pOwner)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("IdleEntryMode");
    pOwner->Get_StateMachine()->Set_Int("IdleEntryMode", 0);

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_AnimName() + "Ani_MainCity_Idle")
        .Loop(true)
        .Apply();
}
