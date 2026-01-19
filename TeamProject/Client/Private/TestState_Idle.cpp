#include "pch.h"
#include "TestState_Idle.h"
#include "TestObject.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "IInputService.h"

void CTestState_Idle::Enter(CTestObject* pOwner)
{
    pOwner->Get_Component<CAnimator3D>()->Change_Animation(2)
        .Loop(true)
        .Apply();

    CCharacterController* pCCt = pOwner->Get_Component<CCharacterController>();
    if (pCCt)
    {
        pCCt->Stop_Movement();
    }
}

void CTestState_Idle::Update(CTestObject* pOwner, _float fTimeDelta)
{

}

void CTestState_Idle::Exit(CTestObject* pOwner)
{
}