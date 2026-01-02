#include "pch.h"
#include "CorinState_Backstep.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Backstep::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Evade_Back")
		.Apply();
}

void CCorinState_Backstep::Update(CCorin* pOwner, _float dt)
{
    _vector3 vDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();
    if (vDelta.x != 0.f || vDelta.z != 0.f)
    {
        _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
        pOwner->Get_CCT()->Move_RootMotion(vDelta, qRot, dt);
    }
}
