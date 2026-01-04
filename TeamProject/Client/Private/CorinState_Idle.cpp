#include "pch.h"
#include "CorinState_Idle.h"

#include "Corin.h"

#include "Animator3D.h"

void CCorinState_Idle::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Idle")
        .Loop(true)
        .Apply();
}