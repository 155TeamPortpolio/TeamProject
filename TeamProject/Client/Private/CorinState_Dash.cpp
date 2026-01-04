#include "pch.h"
#include "CorinState_Dash.h"
#include "Corin.h"

#include "CorinState_Evade.h"
#include "CharacterController.h"

void CCorinState_Dash::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Front")
        .Speed(2.f)
        .Apply();

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CCorinState_Dash::Update(CCorin* pOwner, _float dt)
{
    IHState<CCorin>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;

    pOwner->Process_RootMotion(dt);

    if (pOwner->Is_Attack())
    {   // RushAttack
        pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 3);
        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        return;
    }

    if (m_fAnimProgress >= 0.75f)
    {
        if (pOwner->Is_Move())  // Run
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 2);
        else                    // Idle
            pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 0);

        pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
    }
}
