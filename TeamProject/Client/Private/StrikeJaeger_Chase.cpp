#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Chase::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");

	}
	__super::Enter(pOwner);
}

void CStrikeJaeger_Chase::Update(CStrikeJaeger* pOwner, _float dt)
{
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	if (false == pOwner->GetStateMachine()->Get_Bool("Chase"))
		pOwner->Idle();
}

void CStrikeJaeger_Chase::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CStrikeJaeger_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CStrikeJaeger_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CStrikeJaeger_Run_End::Create());
}

void CStrikeJaeger_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CStrikeJaeger>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CStrikeJaeger_Run_Start::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Run_Start")
		.Apply();
}

void CStrikeJaeger_Run_Start::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Run_Start::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Run_Loop::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Run_Loop")
		.Loop(true)
		.Apply();
}

void CStrikeJaeger_Run_Loop::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Run_Loop::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Run_End::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Run_End")
		.Apply();
}

void CStrikeJaeger_Run_End::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Run_End::Exit(CStrikeJaeger* pOwner)
{
}
