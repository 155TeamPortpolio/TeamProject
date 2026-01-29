#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Chase::Enter(CThugPoacher* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugPoacher>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");

	}
	__super::Enter(pOwner);
}

void CThugPoacher_Chase::Update(CThugPoacher* pOwner, _float dt)
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

void CThugPoacher_Chase::Exit(CThugPoacher* pOwner)
{
}

void CThugPoacher_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CThugPoacher_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CThugPoacher_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugPoacher_Run_End::Create());
}

void CThugPoacher_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CThugPoacher>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CThugPoacher_Run_Start::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Run_Start")
		.Apply();
}

void CThugPoacher_Run_Start::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Run_Start::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Run_Loop::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Run_Loop")
		.Loop(true)
		.Apply();
}

void CThugPoacher_Run_Loop::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Run_Loop::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Run_End::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Run_End")
		.Apply();
}

void CThugPoacher_Run_End::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Run_End::Exit(CThugPoacher* pOwner)
{
}
