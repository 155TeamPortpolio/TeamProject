#include "pch.h"
#include "MiyabiState_Idle.h"
#include "Miyabi.h"
#include "Animator3D.h"

void CMiyabiState_Idle::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Idle")
        .Loop(true)
        .Apply();
}

void CMiyabiState_Idle::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Idle::Exit(CMiyabi* pOwner)
{
}