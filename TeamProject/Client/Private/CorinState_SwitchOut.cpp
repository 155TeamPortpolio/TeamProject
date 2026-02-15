#include "pch.h"
#include "CorinState_SwitchOut.h"

#include "Corin.h"

void CCorinState_SwitchOut::Enter(CCorin* pOwner)
{
    pOwner->Get_StateMachine()->Reset_Trigger("ToIdle");
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchOut_Normal")
        .BlendDuration(0.25f)
        .Speed(1.f)
        .Apply();

    pOwner->Stop_Effect("Corin_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Saw_Slash1");
    pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ultimate_Saw_Slash0");
}

void CCorinState_SwitchOut::Update(CCorin* pOwner, _float dt)
{
    if (pOwner->Is_MainCharacter())
    {
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
        return;
    }

    if (m_fAnimProgress < 0.3f)
    {
        pOwner->Process_RootMotion(dt, ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
    }
    if (m_fAnimProgress >= 0.3f)
    {
        pOwner->Update_DissolveProgress(dt * 5.f);
        pOwner->Process_RootMotion(-dt, ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
            ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
    }

    if (m_fAnimProgress >= 0.8f)
    {
        pOwner->DeActive_Character();
        pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
    }
}

void CCorinState_SwitchOut::Exit(CCorin* pOwner)
{
    pOwner->Get_StateMachine()->Set_Bool("IsMove", false);
    __super::Exit(pOwner);
}

_bool CCorinState_SwitchOut::Handle_Transition(CCorin* pOwner, const string& strState)
{
    if (strState == "Idle")
        return true;
    return false;
}
