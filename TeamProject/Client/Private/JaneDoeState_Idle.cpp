#include "pch.h"
#include "JaneDoeState_Idle.h"

#include "JaneDoe.h"

#include "Animator3D.h"

void CJaneDoeState_Idle::Enter(CJaneDoe* pOwner)
{
    pOwner->Unlock_Move();
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("IdleEntryMode");
    pOwner->Get_StateMachine()->Set_Int("IdleEntryMode", 0);

    pOwner->End_AllAttackColliders();

    if (iEntryMode == 1)
    {
        pOwner->Get_StateMachine()->Set_Trigger("Evade");
    }

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Idle")
        .Loop(true)
        .Apply();

    __super::Enter(pOwner);
}

void CJaneDoeState_Idle::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CJaneDoeState_Idle::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}