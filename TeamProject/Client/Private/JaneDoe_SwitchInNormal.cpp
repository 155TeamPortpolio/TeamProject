#include "pch.h"
#include "JaneDoe_SwitchInNormal.h"
#include "JaneDoe.h"

void CJaneDoe_SwitchInNormal::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Normal")
        .Speed(1.f)
        .Apply();

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CJaneDoe_SwitchInNormal::Update(CJaneDoe* pOwner, _float dt)
{
    IHState<CJaneDoe>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt);

    if (m_fAnimProgress >= 1.0f)
    {   
        pSwitch->Get_SubStateMachine()->Set_Int("ExitMode", 0);
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
    }
}
