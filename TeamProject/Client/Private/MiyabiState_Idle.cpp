#include "pch.h"
#include "MiyabiState_Idle.h"
#include "Miyabi.h"
#include "Animator3D.h"

void CMiyabiState_Idle::Enter(CMiyabi* pOwner)
{
    pOwner->Unlock_Move();
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("IdleEntryMode");
    pOwner->Get_StateMachine()->Set_Int("IdleEntryMode", 0);

    pOwner->End_AllAttackColliders();

    if (iEntryMode == 1)
    {
        pOwner->Get_StateMachine()->Set_Trigger("Evade");
    }
    else if (iEntryMode == 2)
    {
        pOwner->Set_Switch(CCharacter::SWITCH::PARRYAID);
        pOwner->Get_StateMachine()->Set_Trigger("SwitchIn");
    }

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Idle")
        .Loop(true)
        .Apply();

    __super::Enter(pOwner);
}

void CMiyabiState_Idle::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CMiyabiState_Idle::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}