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
    for (_uint i = 0; i < 3; i++)
        pOwner->Get_Component<CAnimator3D>()->Set_Animation(i, 9);
}

void CTestState_Walk::Update(CTestObject* pOwner, _float fTimeDelta)
{
    IInputService* pInput = CGameInstance::GetInstance()->Get_InputDev();
    CStateMachine<CTestObject>* pStateMachine = pOwner->Get_StateMachine();
    CCharacterController* pCCT = pOwner->Get_Component<CCharacterController>();
    CTransform* pTransform = pOwner->Get_Component<CTransform>();

    // 지상 체크
    _bool bGround = false;
    if (pCCT)
        bGround = pCCT->Is_Grounded();
    pStateMachine->Set_Bool("IsGrounded", bGround);

    // 월드 축 기준 입력 수집 (WASD)
    _vector3 vMoveDir = _vector3(0.f, 0.f, 0.f);

    if (pInput->Key_Down('W'))
        vMoveDir.z += 1.f;  // +Z 방향
    if (pInput->Key_Down('S'))
        vMoveDir.z -= 1.f;  // -Z 방향
    if (pInput->Key_Down('D'))
        vMoveDir.x += 1.f;  // +X 방향
    if (pInput->Key_Down('A'))
        vMoveDir.x -= 1.f;  // -X 방향

    // 이동 여부 체크
    _bool bMove = vMoveDir.Length() > 0.01f;
    pStateMachine->Set_Bool("IsMoving", bMove);

    // 이동 처리
    if (bMove && pTransform && pCCT)
    {
        vMoveDir.Normalize();

        _vector3 vPos = pTransform->Dir(STATE::POSITION);
        pTransform->LookAt(vPos + vMoveDir);

        // 이동
        pCCT->Move_Direction(vMoveDir, m_fWalkSpeed, fTimeDelta);
    }

    // 점프 입력
    if (pInput->Key_Down('J'))
        pStateMachine->Set_Trigger("Jump");
}

void CTestState_Walk::Exit(CTestObject* pOwner)
{
}