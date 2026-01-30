#include "pch.h"
#include "CorinState_SwitchInNormal.h"
#include "Corin.h"

void CCorinState_SwitchInNormal::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SiwtchIn_Normal")
        .Speed(1.f)
        .Apply();

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CCorinState_SwitchInNormal::Update(CCorin* pOwner, _float dt)
{
    IHState<CCorin>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt, ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

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

void CCorinState_SwitchInNormal::Exit(CCorin* pOwner)
{
    __super::Exit(pOwner);
}
