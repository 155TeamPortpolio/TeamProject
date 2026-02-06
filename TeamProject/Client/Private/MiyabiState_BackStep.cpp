#include "pch.h"
#include "MiyabiState_Backstep.h"
#include "Miyabi.h"

#include "CharacterController.h"

void CMiyabiState_BackStep::Enter(CMiyabi* pOwner)
{
    pOwner->Use_Evade();
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Back")
        .Apply();
}

void CMiyabiState_BackStep::Update(CMiyabi* pOwner, _float dt)
{
    IHState<CMiyabi>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;
    auto pSubMachine = pEvade->Get_SubStateMachine();

    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

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

    if (m_fAnimProgress >= 0.12f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {   // Idle -> Evade
            pSubMachine->Set_Int("ExitMode", 4);
            pSubMachine->Set_Trigger("Complete");
            return;
        }
    }

    if (m_fAnimProgress >= 0.25f)
    {   // Run
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
}
