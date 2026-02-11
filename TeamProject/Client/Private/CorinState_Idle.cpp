#include "pch.h"
#include "CorinState_Idle.h"

#include "Corin.h"

#include "Animator3D.h"

void CCorinState_Idle::Enter(CCorin* pOwner)
{
    pOwner->Unlock_Move();
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("IdleEntryMode");
    pOwner->Get_StateMachine()->Set_Int("IdleEntryMode", 0);

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

    pOwner->Stop_Effect("Corin_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ultimate_Saw_Slash0");
}

void CCorinState_Idle::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CCorinState_Idle::Exit(CCorin* pOwner)
{
    __super::Exit(pOwner);
}
