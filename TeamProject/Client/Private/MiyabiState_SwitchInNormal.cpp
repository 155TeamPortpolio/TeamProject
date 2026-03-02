#include "pch.h"
#include "MiyabiState_SwitchInNormal.h"
#include "Miyabi.h"

void CMiyabiState_SwitchInNormal::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Normal")
        .Speed(1.f)
        .Apply();

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CMiyabiState_SwitchInNormal::Update(CMiyabi* pOwner, _float dt)
{
    IHState<CMiyabi>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    if (pOwner->Is_Attack())
    {
        pSwitch->Get_SubStateMachine()->Set_Int("ExitMode", 2);
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (pOwner->Is_Evade())
    {
        pSwitch->Get_SubStateMachine()->Set_Int("ExitMode", 3);
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (pOwner->Is_Move_Buffer())
    {
        pSwitch->Get_SubStateMachine()->Set_Int("ExitMode", 1);
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (m_fAnimProgress >= 1.0f)
    {
        pSwitch->Get_SubStateMachine()->Set_Int("ExitMode", 0);
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
    }
}

void CMiyabiState_SwitchInNormal::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}
