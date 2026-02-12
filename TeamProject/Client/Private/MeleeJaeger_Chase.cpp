#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Chase::Enter(CMeleeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CMeleeJaeger>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");
		__super::Enter(pOwner);
	}

	if (false == pOwner->IsShield())
		__super::Enter(pOwner);
	else
		m_pSubStateMachine->Change_State("Walk_Front");
}

void CMeleeJaeger_Chase::Update(CMeleeJaeger* pOwner, _float dt)
{
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	//if (false == pOwner->GetStateMachine()->Get_Bool("Chase"))
	//	m_pSubStateMachine->Change_State("Run_End");
		//pOwner->Idle();
}

void CMeleeJaeger_Chase::Exit(CMeleeJaeger* pOwner)
{
}

void CMeleeJaeger_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CMeleeJaeger_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CMeleeJaeger_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CMeleeJaeger_Run_End::Create());
	m_pSubStateMachine->Register_State("Walk_Front", CMeleeJaeger_Chase_Walk_Front::Create());
}

void CMeleeJaeger_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CMeleeJaeger>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CMeleeJaeger_Run_Start::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Run_Start")
		.Apply();
}

void CMeleeJaeger_Run_Start::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Run_Start::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Run_Loop::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Run_Loop")
		.Loop(true)
		.Apply();
}

void CMeleeJaeger_Run_Loop::Update(CMeleeJaeger* pOwner, _float dt)
{
	if (false == pOwner->GetStateMachine()->Get_Bool("Chase"))
		m_pOwnerStateMachine->Change_State("Run_End");
}

void CMeleeJaeger_Run_Loop::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Run_End::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Run_End")
		.Apply();
}

void CMeleeJaeger_Run_End::Update(CMeleeJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CMeleeJaeger_Run_End::Exit(CMeleeJaeger* pOwner)
{

}

/*============================================================================*/
void CMeleeJaeger_Chase_Walk_Front::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_F")
		.Speed(1.3)
		.Loop(true)
		.Apply();
}

void CMeleeJaeger_Chase_Walk_Front::Update(CMeleeJaeger* pOwner, _float dt)
{
	if (false == pOwner->GetStateMachine()->Get_Bool("Chase"))
		pOwner->Idle();
}

void CMeleeJaeger_Chase_Walk_Front::Exit(CMeleeJaeger* pOwner)
{
}
