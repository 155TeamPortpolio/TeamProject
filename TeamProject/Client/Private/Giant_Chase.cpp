#include "pch.h"
#include "Giant.h"
#include "Giant_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Chase::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");

	}
	__super::Enter(pOwner);
}

void CGiant_Chase::Update(CGiant* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);
}

void CGiant_Chase::Exit(CGiant* pOwner)
{
}

void CGiant_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CGiant_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CGiant_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CGiant_Run_End::Create());
}

void CGiant_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CGiant>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CGiant_Run_Start::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Run_Start")
		.Apply();
}

void CGiant_Run_Start::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Run_Start::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Run_Loop::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Run_loop")
		.Loop(true)
		.Apply();
}

void CGiant_Run_Loop::Update(CGiant* pOwner, _float dt)
{
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	if (false == pOwner->GetStateMachine()->Get_Bool("Chase"))
		m_pOwnerStateMachine->Change_State("Run_End");
}

void CGiant_Run_Loop::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Run_End::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Run_End")
		.Apply();
}

void CGiant_Run_End::Update(CGiant* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CGiant_Run_End::Exit(CGiant* pOwner)
{
}
