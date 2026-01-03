#include "pch.h"
#include "CorinState_Dash.h"
#include "Corin.h"

#include "CorinState_Evade.h"
#include "CharacterController.h"

void CCorinState_Dash::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run_Start")
        .Speed(2.f)
        .Apply();

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);
}

void CCorinState_Dash::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    if (m_fAnimProgress >= 0.75f)
    {
        IHState<CCorin>* pEvade = Get_ParentState();
        if (pEvade && pEvade->Get_SubStateMachine())
        {
            if (pOwner->Is_Move())
                pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 2);
            else
                pEvade->Get_SubStateMachine()->Set_Int("ExitMode", 1);
            pEvade->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
}
