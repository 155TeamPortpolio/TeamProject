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
        pOwner->Rotate_Horizontal(-vInputDir, dt);

        auto pCCT = pOwner->Get_Component<CCharacterController>();
        if (pCCT)
            pCCT->Move_Direction(vInputDir, pOwner->Get_Speed(), dt);
    }
}

void CTestState_Jump::Exit(CTestObject* pOwner)
{
}