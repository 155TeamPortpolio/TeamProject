#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Chase::Enter(CCyclops* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CCyclops>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");

	}
	__super::Enter(pOwner);
}

void CCyclops_Chase::Update(CCyclops* pOwner, _float dt)
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

void CCyclops_Chase::Exit(CCyclops* pOwner)
{
}

void CCyclops_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CCyclops_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CCyclops_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CCyclops_Run_End::Create());
}

void CCyclops_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CCyclops>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CCyclops_Run_Start::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Run_Start")
		.Apply();
}

void CCyclops_Run_Start::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Run_Start::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Run_Loop::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Run_Loop")
		.Apply();
}

void CCyclops_Run_Loop::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Run_Loop::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Run_End::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Run_End")
		.Apply();
}

void CCyclops_Run_End::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Run_End::Exit(CCyclops* pOwner)
{
}
