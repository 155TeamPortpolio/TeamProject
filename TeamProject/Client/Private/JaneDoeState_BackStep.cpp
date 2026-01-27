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
    auto pSubMachine = pEvade->Get_SubStateMachine();

    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    if (pOwner->Is_Attack())
    {   
        if (pSubMachine->Get_Bool("Extreme"))
        {
            if (pOwner->Is_Passion())
            {
                // 열광상태 회피반격
            }
            else
            {
                // 회피반격 : 1단,2단 구분
                if (m_bEvadeType)
                {
                    // 1단
                }
                else
                {
                    // 2단
                }
            }
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

    if (m_fAnimProgress >= 0.3f)
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
