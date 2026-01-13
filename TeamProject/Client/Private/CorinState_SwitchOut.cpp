#include "pch.h"
#include "CorinState_SwitchOut.h"

#include "Corin.h"

void CCorinState_SwitchOut::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchOut_Normal")
        .Loop(false)
        .Apply();
}

void CCorinState_SwitchOut::Update(CCorin* pOwner, _float dt)
{
    if (m_fAnimProgress >= 0.3f)
        pOwner->Update_DissolveProgress(dt * 5.f);

    pOwner->Process_RootMotion(-dt, ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.6f)
    {
        pOwner->Reset_DissolveProgress();
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
    }
}
