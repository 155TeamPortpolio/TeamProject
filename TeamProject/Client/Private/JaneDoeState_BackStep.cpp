#include "pch.h"
#include "JaneDoeState_Backstep.h"
#include "JaneDoe.h"

#include "CharacterController.h"

void CJaneDoeState_BackStep::Enter(CJaneDoe* pOwner)
{    
    pOwner->Use_Evade();
    if (pOwner->Is_Passion())
    {
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Back_03")
            .Speed(1.f)
            .Apply();
    }
    else
    {
        m_bEvadeType = !m_bEvadeType;

        string strEvade = m_bEvadeType ? "Evade_Back_01" : "Evade_Back_02";
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + strEvade)
            .Speed(1.f)
            .Apply();
    }
}

void CJaneDoeState_BackStep::Update(CJaneDoe* pOwner, _float dt)
{
    IHState<CJaneDoe>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    if (pOwner->Is_Attack())
    {   // RushAttack
        pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 3);
        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (m_fAnimProgress >= 0.12f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {   // Idle -> Evade
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 4);
            pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
            return;
        }
    }

    if (m_fAnimProgress >= 0.5f)
    {   // Run
        if (pOwner->Is_Move())
        {
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 2);
            pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
            return;
        }
    }

    if (m_fAnimProgress >= 0.7f)
    {   // Idle
        pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 0);
        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
    }
}
