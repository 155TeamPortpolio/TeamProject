#include "pch.h"
#include "JaneDoeState_Dash.h"
#include "JaneDoe.h"

#include "JaneDoeState_Evade.h"
#include "CharacterController.h"

void CJaneDoeState_Dash::Enter(CJaneDoe* pOwner)
{
    pOwner->Use_Evade();
    if (pOwner->Is_Passion())
    {
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Front_03")
            .Speed(1.f)
            .Apply();
    }
    else
    {
        m_bEvadeType = !m_bEvadeType;

        string strEvade = m_bEvadeType ? "Evade_Front_01" : "Evade_Front_02";
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + strEvade)
            .Speed(1.f)
            .Apply();
    }

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CJaneDoeState_Dash::Update(CJaneDoe* pOwner, _float dt)
{
    IHState<CJaneDoe>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt);

    if (pOwner->Is_Attack())
    {   // RushAttack
        pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 3);
        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (m_fAnimProgress >= 0.15f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {   // Idle -> Evade
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 4);
            pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
            return;
        }

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
