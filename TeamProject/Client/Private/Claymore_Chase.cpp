#include "pch.h"
#include "Claymore.h"
#include "Claymore_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Chase::Enter(CClaymore* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CClaymore>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");

	}
	__super::Enter(pOwner);
}

void CClaymore_Chase::Update(CClaymore* pOwner, _float dt)
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

void CClaymore_Chase::Exit(CClaymore* pOwner)
{
}

void CClaymore_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CClaymore_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CClaymore_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CClaymore_Run_End::Create());
}

void CClaymore_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CClaymore>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CClaymore_Run_Start::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Run_Start")
		.Apply();
}

void CClaymore_Run_Start::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Run_Start::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Run_Loop::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Run_Loop")
		.Apply();
}

void CClaymore_Run_Loop::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Run_Loop::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Run_End::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Run_End")
		.Apply();
}

void CClaymore_Run_End::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Run_End::Exit(CClaymore* pOwner)
{
}
