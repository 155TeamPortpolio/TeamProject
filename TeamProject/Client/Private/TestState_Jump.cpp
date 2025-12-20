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
        pCCT->Jump(m_fJumpPower);
    }
}

void CTestState_Jump::Update(CTestObject* pOwner, _float fTimeDelta)
{
    CStateMachine<CTestObject>* pStateMachine = pOwner->Get_StateMachine();
    CCharacterController* pCCT = pOwner->Get_Component<CCharacterController>();
    CTransform* pTransform = pOwner->Get_Component<CTransform>();
    IInputService* pInput = CGameInstance::GetInstance()->Get_InputDev();

    // 지상 체크
    _bool bGround = false;
    if (pCCT)
        bGround = pCCT->Is_Grounded();
    pStateMachine->Set_Bool("IsGrounded", bGround);

    // 공중 이동 제어
    _vector3 vMoveDir = _vector3(0.f, 0.f, 0.f);

    if (pInput->Key_Down('W'))
        vMoveDir.z += 1.f;
    if (pInput->Key_Down('S'))
        vMoveDir.z -= 1.f;
    if (pInput->Key_Down('D'))
        vMoveDir.x += 1.f;
    if (pInput->Key_Down('A'))
        vMoveDir.x -= 1.f;

    if (vMoveDir.Length() > 0.01f && pCCT && pTransform)
    {
        vMoveDir.Normalize();

        // 회전
        _vector3 vLook = pTransform->Dir(STATE::LOOK);
        _vector3 vNewLook = _vector3::Lerp(vLook, vMoveDir, 10.f * fTimeDelta);
        vNewLook.Normalize();

        _vector3 vPos = pTransform->Dir(STATE::POSITION);
        pTransform->LookAt(vPos + vNewLook);

        // 공중 이동
        pCCT->Move_Direction(vMoveDir, m_fWalkSpeed, fTimeDelta);
    }

    // 공중에서도 점프 가능
    if (pInput->Key_Down('J'))
        pStateMachine->Set_Trigger("Jump");
}

void CTestState_Jump::Exit(CTestObject* pOwner)
{
}