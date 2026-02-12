#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Move::Enter(CMeleeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CMeleeJaeger>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	auto pOwnerStateMachine = pOwner->GetStateMachine();

	_int iMovePatternIndex = {};

	if (0 != pOwner->GetStateMachine()->Get_Int("MovePattern")) {
		iMovePatternIndex = pOwner->GetStateMachine()->Get_Int("MovePattern");
		pOwner->GetStateMachine()->Set_Int("MovePattern", 0);
	}
	else {
		auto hysteriesis = pOwner->GetHysteriesis();
		_float fDistance = pOwner->GetTargetingInfo().fDistance;

		// 거리기반 행동판단해야함
		if (fDistance <= hysteriesis.fEvadeEnter)
			iMovePatternIndex = MOVEINDEX::Walk_Back;
		else if (fDistance <= hysteriesis.fComboExit)
			iMovePatternIndex = Helper::Get_Random_Int(3, 7);
		else
			iMovePatternIndex = MOVEINDEX::Walk_Front;
	}
	ChangeMovePatternFromIndex(iMovePatternIndex);
}

void CMeleeJaeger_Move::Update(CMeleeJaeger* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CMeleeJaeger_Move::Exit(CMeleeJaeger* pOwner)
{
}

void CMeleeJaeger_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CMeleeJaeger_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CMeleeJaeger_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CMeleeJaeger_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CMeleeJaeger_Walk_Right::Create());
	//m_pSubStateMachine->Register_State("Walk_F_L", CMeleeJaeger_Walk_F_L::Create());
	m_pSubStateMachine->Register_State("Walk_F_R", CMeleeJaeger_Walk_F_R::Create());
	m_pSubStateMachine->Register_State("Walk_R_F", CMeleeJaeger_Walk_R_F::Create());
}

void CMeleeJaeger_Move::Register_Transitions()
{
}


void CMeleeJaeger_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CMeleeJaeger_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CMeleeJaeger_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CMeleeJaeger_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CMeleeJaeger_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	//case Client::CMeleeJaeger_Move::Walk_F_L:
	//	m_pSubStateMachine->Change_State("Walk_F_L");
	//	break;
	case Client::CMeleeJaeger_Move::Walk_F_R:
		m_pSubStateMachine->Change_State("Walk_F_R");
		break;
	case Client::CMeleeJaeger_Move::Walk_R_F:
		m_pSubStateMachine->Change_State("Walk_R_F");
		break;
	}
}

/*============================================================================*/
void CMeleeJaeger_Walk_Front::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_F")
		.Apply();
}

void CMeleeJaeger_Walk_Front::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Walk_Front::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Walk_Back::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_B")
		.Apply();
}

void CMeleeJaeger_Walk_Back::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Walk_Back::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Walk_Left::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_L")
		.Apply();
}

void CMeleeJaeger_Walk_Left::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Walk_Left::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Walk_Right::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_R")
		.Apply();
}

void CMeleeJaeger_Walk_Right::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Walk_Right::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
//void CMeleeJaeger_Walk_F_L::Enter(CMeleeJaeger* pOwner)
//{
//	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_FL_RFoot")
//		.Apply();
//}
//
//void CMeleeJaeger_Walk_F_L::Update(CMeleeJaeger* pOwner, _float dt)
//{
//}
//
//void CMeleeJaeger_Walk_F_L::Exit(CMeleeJaeger* pOwner)
//{
//}

/*============================================================================*/
void CMeleeJaeger_Walk_F_R::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_F_R")
		.Apply();
}

void CMeleeJaeger_Walk_F_R::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Walk_F_R::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Walk_R_F::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Move_R_F")
		.Apply();
}

void CMeleeJaeger_Walk_R_F::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Walk_R_F::Exit(CMeleeJaeger* pOwner)
{
}
