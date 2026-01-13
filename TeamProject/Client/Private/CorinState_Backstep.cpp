#include "pch.h"
#include "CorinState_Backstep.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Backstep::Enter(CCorin* pOwner)
{
    pOwner->Use_Evade();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Back")
        .Speed(1.f)
		.Apply();
}

void CCorinState_Backstep::Update(CCorin* pOwner, _float dt)
{
    IHState<CCorin>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (pOwner->Is_Attack())
    {   // RushAttack
        pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 3);
        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (m_fAnimProgress >= 0.25f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {
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
