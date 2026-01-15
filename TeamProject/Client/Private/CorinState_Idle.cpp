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

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Idle")
        .Loop(true)
        .Apply();
}

void CCorinState_Idle::Update(CCorin* pOwner, _float dt)
{
    //pOwner->Process_RootMotion(dt,
    //    ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
    //    ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}
