#include "pch.h"
#include "MiyabiState_Idle.h"
#include "Miyabi.h"
#include "Miyabi_Ghost.h"
#include "Animator3D.h"

void CMiyabiState_Idle::Enter(CMiyabi* pOwner)
{
    pOwner->Set_WeaponEffectMesh(false);
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
        .BlendDuration(0.5f)
        .Loop(true)
        .Apply();

    pOwner->Show_Ghost();
    pOwner->Get_Ghost()->Get_Component<CAnimator3D>()
        ->Change_Animation(pOwner->Get_GhostName() + "Idle")
        .Loop(true)
        .Apply();

    __super::Enter(pOwner);
}

void CMiyabiState_Idle::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    pOwner->Get_StateMachine()->Reset_Trigger("ResetState");
}

void CMiyabiState_Idle::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}