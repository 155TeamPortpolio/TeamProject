#include "pch.h"
#include "JaneDoeState_SwitchOut.h"

#include "JaneDoe.h"

void CJaneDoeState_SwitchOut::Enter(CJaneDoe* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();
    pOwner->Stop_Rotation();
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchOut_Normal")
        .Loop(false)
        .Apply();
}

void CJaneDoeState_SwitchOut::Update(CJaneDoe* pOwner, _float dt)
{
    if (m_fAnimProgress >= 0.3f)
        pOwner->Update_DissolveProgress(dt * 5.f);

    pOwner->Process_RootMotion(-dt, ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.6f)
    {
        pOwner->Reset_DissolveProgress();
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
    }
}

void CJaneDoeState_SwitchOut::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}
