#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Chase::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");
		
	}
	__super::Enter(pOwner);
}

void CThugAssaulter_Chase::Update(CThugAssaulter* pOwner, _float dt)
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

void CThugAssaulter_Chase::Exit(CThugAssaulter* pOwner)
{
}

void CThugAssaulter_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CThugAssaulter_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run_Loop", CThugAssaulter_Run_Loop::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugAssaulter_Run_End::Create());
}

void CThugAssaulter_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run_Loop",
		CStateMachine<CThugAssaulter>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CThugAssaulter_Run_Start::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Run_Start")
		.Apply();
}

void CThugAssaulter_Run_Start::Update(CThugAssaulter* pOwner, _float dt)
{

}

void CThugAssaulter_Run_Start::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Run_Loop::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Run_Loop")
		.Loop(true)
		.Apply();
}

void CThugAssaulter_Run_Loop::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Run_Loop::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Run_End::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Run_End")
		.Apply();
}

void CThugAssaulter_Run_End::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Run_End::Exit(CThugAssaulter* pOwner)
{
}
