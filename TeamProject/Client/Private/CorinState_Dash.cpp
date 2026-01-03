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
    _vector3 vDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();
    if (vDelta.x != 0.f || vDelta.z != 0.f)
    {
        _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
        pOwner->Get_CCT()->Move_RootMotion(vDelta, qRot, dt);
    }
}
