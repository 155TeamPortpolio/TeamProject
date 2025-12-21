#include "pch.h"
#include "TestState_Jump.h"
#include "TestObject.h"
#include "Animator3D.h"
#include "Transform.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "IInputService.h"

void CTestState_Jump::Enter(CTestObject* pOwner)
{
    pOwner->Get_Component<CAnimator3D>()->Set_Animation(0, 1);

    CCharacterController* pCCT = pOwner->Get_Component<CCharacterController>();
    if (pCCT)
    {
        pCCT->Jump(pOwner->Get_JumpPower());
    }
}

void CTestState_Jump::Update(CTestObject* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();

    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();

        CTransform* pTransform = pOwner->Get_Component<CTransform>();

        _vector3 vLook = pTransform->Dir(STATE::LOOK);
        _vector3 vNewLook = _vector3::Lerp(vLook, vInputDir, 10.f * dt);
        vNewLook.Normalize();

        _vector3 vPos = pTransform->Dir(STATE::POSITION);
        pTransform->LookAt(vPos + vNewLook);

        pOwner->Get_Component<CCharacterController>()->Move_Direction(vInputDir, pOwner->Get_Speed(), dt);
    }
}

void CTestState_Jump::Exit(CTestObject* pOwner)
{
}