#include "pch.h"
#include "JaneDoeState_SwitchOut.h"

#include "JaneDoe.h"

void CJaneDoeState_SwitchOut::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchOut_Normal")
        .Loop(false)
        .Apply();
}

void CJaneDoeState_SwitchOut::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    if (m_fAnimProgress >= 1.f)
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
}
