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
    pOwner->Update_DissolveProgress(dt);

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.4f)
    {
        pOwner->Reset_DissolveProgress();
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
    }
}
