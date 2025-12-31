#include "pch.h"
#include "AnbiState_Idle.h"

#include "Anbi.h"

#include "Animator3D.h"

void CAnbiState_Idle::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Idle")
        .Loop(true)
        .Apply();
}
