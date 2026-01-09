#include "pch.h"
#include "ThugBulkyEnforcer.h"
#include "ThugBulkyEnforcer_Chase.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugBulkyEnforcer_Chase::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();
		Register_States();
		Register_Transitions();
		m_pSubStateMachine->Set_DefaultState("Run_Start");
	}

	__super::Enter(pOwner);
}

void CThugBulkyEnforcer_Chase::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	// 매프레임 추적
	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);

	if (false == pOwner->Get_StateMachine()->Get_Bool("Chase")) 
		pOwner->Idle();
}

void CThugBulkyEnforcer_Chase::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Chase::Register_States()
{
	m_pSubStateMachine->Register_State("Run_Start", CThugBulkyEnforcer_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run", CThugBulkyEnforcer_Run::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugBulkyEnforcer_Run_End::Create());
}

void CThugBulkyEnforcer_Chase::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("Run_Start", "Run",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);

}


/*============================================================================*/
void CThugBulkyEnforcer_Run_Start::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Run_Start")
		.Apply();

	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);
}

void CThugBulkyEnforcer_Run_Start::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
	
}

void CThugBulkyEnforcer_Run_Start::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Run::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Run")
		.Loop(true)
		.Apply();
	m_vFIndPlayerPosTime = { 1.f, 0.f };
}

void CThugBulkyEnforcer_Run::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	//m_vFIndPlayerPosTime.y += dt;
	//if (m_vFIndPlayerPosTime.y >= m_vFIndPlayerPosTime.x) {
	//	m_vFIndPlayerPosTime.y = 0.f;
	//}
	//pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);
		


	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Run::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Run_End::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Run_End")
		.Apply();
}

void CThugBulkyEnforcer_Run_End::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Run_End::Exit(CThugBulkyEnforcer* pOwner)
{
}

