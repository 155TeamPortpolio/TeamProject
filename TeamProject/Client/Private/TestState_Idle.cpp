#include "pch.h"
#include "TestState_Idle.h"
#include "TestObject.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "IInputService.h"

void CTestState_Idle::Enter(CTestObject* pOwner)
{
    for (_uint i = 0; i < 3; i++)
        pOwner->Get_Component<CAnimator3D>()->Set_Animation(i, 2);

    CCharacterController* pCCt = pOwner->Get_Component<CCharacterController>();
    if (pCCt)
    {
        pCCt->Stop_Movement();
    }
}

void CTestState_Idle::Update(CTestObject* pOwner, _float fTimeDelta)
{
    CStateMachine<CTestObject>* pStateMachine = pOwner->Get_StateMachine();
    CCharacterController* pCCt = pOwner->Get_Component<CCharacterController>();
    IInputService* pInput = CGameInstance::GetInstance()->Get_InputDev();

    // 지상 체크
    _bool bGround = false;
    if (pCCt)
        bGround = pCCt->Is_Grounded();
    pStateMachine->Set_Bool("IsGrounded", bGround);

    // 이동 입력 체크
    _bool bMove = false;
    if (pInput->Key_Down('W') ||
        pInput->Key_Down('A') ||
        pInput->Key_Down('S') ||
        pInput->Key_Down('D'))
    {
        bMove = true;
    }
    pStateMachine->Set_Bool("IsMoving", bMove);

    // 점프 입력
    if (pInput->Key_Down('J'))
        pStateMachine->Set_Trigger("Jump");
}

void CTestState_Idle::Exit(CTestObject* pOwner)
{
}