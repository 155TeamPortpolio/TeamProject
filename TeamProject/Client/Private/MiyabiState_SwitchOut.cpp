#include "pch.h"
#include "MiyabiState_SwitchOut.h"

#include "Miyabi.h"

void CMiyabiState_SwitchOut::Enter(CMiyabi* pOwner)
{
    pOwner->Get_StateMachine()->Reset_Trigger("ToIdle");
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchOut_Normal")
        .Loop(false)
        .Apply();
}

void CMiyabiState_SwitchOut::Update(CMiyabi* pOwner, _float dt)
{
    if (pOwner->Is_MainCharacter())
    {
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
        return;
    }

    if (m_fAnimProgress >= 0.3f)
        pOwner->Update_DissolveProgress(dt * 5.f);

    pOwner->Process_RootMotion(-dt, ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.6f)
    {
        pOwner->DeActive_Character();
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
    }
}

void CMiyabiState_SwitchOut::Exit(CMiyabi* pOwner)
{
    pOwner->Get_StateMachine()->Set_Bool("IsMove", false);
    __super::Exit(pOwner);
}

_bool CMiyabiState_SwitchOut::Handle_Transition(CMiyabi* pOwner, const string& strState)
{
    if (strState == "Idle")
        return true;
    return false;
}
