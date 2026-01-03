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
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.5f)
    {
        IHState<CCorin>* pEvade = Get_ParentState();
        if (pEvade && pEvade->Get_SubStateMachine())
        {
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 0);
            pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
}
