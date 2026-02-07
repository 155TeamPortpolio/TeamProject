#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Parried.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

//void CStrikeJaeger_Parried::Enter(CStrikeJaeger* pOwner)
//{
//	if (nullptr == m_pSubStateMachine) {
//		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();
//
//		Register_States();
//		Register_Transitions();
//
//		__super::Enter(pOwner);
//	}
//
//
//}
//
//void CStrikeJaeger_Parried::Update(CStrikeJaeger* pOwner, _float dt)
//{
//	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
//	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
//	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
//		vRootBoneMoveDelta,
//		qRot,
//		dt);
//
//	__super::Update(pOwner, dt);
//
//	if (m_fAnimProgress > 0.99f)
//		pOwner->Idle();
//}
//
//void CStrikeJaeger_Parried::Exit(CStrikeJaeger* pOwner)
//{
//}
//
//void CStrikeJaeger_Parried::Register_States()
//{
//	//m_pSubStateMachine->Register_State("Dodge", CStrikeJaeger_Dodge::Create());
//	//m_pSubStateMachine->Register_State("Dodge_L", CStrikeJaeger_Dodge_L::Create());
//	//m_pSubStateMachine->Register_State("Dodge_R", CStrikeJaeger_Dodge_R::Create());
//}
//
//void CStrikeJaeger_Parried::Register_Transitions()
//{
//}

/*============================================================================*/

void CStrikeJaeger_Parried::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Hit_H_Front")
		.Apply();
}

void CStrikeJaeger_Parried::Update(CStrikeJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CStrikeJaeger_Parried::Exit(CStrikeJaeger* pOwner)
{
}
