#include "pch.h"
#include "CorinState_Dash.h"
#include "Corin.h"

#include "CorinState_Evade.h"
#include "CharacterController.h"

void CCorinState_Dash::Enter(CCorin* pOwner)
{
    pOwner->Use_Evade();
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Front")
        .Speed(1.f)
        .Apply();

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CCorinState_Dash::Update(CCorin* pOwner, _float dt)
{
    IHState<CCorin>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;
    auto pSubMachine = pEvade->Get_SubStateMachine();

    pOwner->Process_RootMotion(dt);

    if (pOwner->Is_Attack())
    {
        if (pSubMachine->Get_Bool("Extreme"))
        {
            // CounterAttack
            pSubMachine->Set_Int("ExitMode", 5);
            pSubMachine->Set_Trigger("Complete");
        }
        else
        {
            // RushAttack
            pSubMachine->Set_Int("ExitMode", 3);
            pSubMachine->Set_Trigger("Complete");
        }
        return;
    }

    if (m_fAnimProgress >= 0.15f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {   // Idle -> Evade
            pSubMachine->Set_Int("ExitMode", 4);
            pSubMachine->Set_Trigger("Complete");
            return;
        }

        if (pOwner->Is_Move())
        {
            pSubMachine->Set_Int("ExitMode", 2);
            pSubMachine->Set_Trigger("Complete");
            return;
        }
    }

    if (m_fAnimProgress >= 0.7f)
    {   // Idle
        pSubMachine->Set_Int("ExitMode", 0);
        pSubMachine->Set_Trigger("Complete");
    }

    Update_Effects(pOwner);
}

void CCorinState_Dash::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.02f))
    {
        pOwner->Play_Effect("Player_Run_Start0", _vector3(0.f, 1.1f, 1.3f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Player_Run_Start1", _vector3(0.f, 0.15f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }
}
