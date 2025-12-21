#include "pch.h"
#include "TestState_Walk.h"
#include "TestObject.h"
#include "Animator3D.h"
#include "Transform.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "IInputService.h"

void CTestState_Walk::Enter(CTestObject* pOwner)
{
    pOwner->Get_Component<CAnimator3D>()->Set_Animation(0, 9);
}

void CTestState_Walk::Update(CTestObject* pOwner, _float fTimeDelta)
{
    _vector3 vInputDir = pOwner->Get_InputDir();

    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();

        CTransform* pTransform = pOwner->Get_Component<CTransform>();
        _vector3 vPos = pTransform->Dir(STATE::POSITION);
        pTransform->LookAt(vPos + vInputDir);

        // ÀÌµ¿
        pOwner->Get_Component<CCharacterController>()->Move_Direction(vInputDir, pOwner->Get_Speed(), fTimeDelta);
    }
}

void CTestState_Walk::Exit(CTestObject* pOwner)
{
}