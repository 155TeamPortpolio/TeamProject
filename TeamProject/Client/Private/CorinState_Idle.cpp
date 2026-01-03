#include "pch.h"
#include "CorinState_Idle.h"

#include "Corin.h"

#include "Animator3D.h"

void CCorinState_Idle::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Idle")
        .Loop(true)
        .Apply();
}