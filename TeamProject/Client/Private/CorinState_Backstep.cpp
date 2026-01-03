#include "pch.h"
#include "CorinState_Backstep.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Backstep::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Evade_Back")
        .Speed(2.f)
		.Apply();
}

void CCorinState_Backstep::Update(CCorin* pOwner, _float dt)
{
    IHState<CCorin>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    pOwner->Process_RootMotion(dt);

    if (pOwner->Is_Attack())
    {   // RushAttack
        pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 3);
        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (m_fAnimProgress >= 0.5f)
    {
        if (pOwner->Is_Move())  // Run
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 2);
        else                    // Idle
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 0);

        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
    }
}
