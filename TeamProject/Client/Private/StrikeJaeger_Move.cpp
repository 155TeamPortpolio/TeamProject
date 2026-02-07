#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Move::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();

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

void CStrikeJaeger_Move::Update(CStrikeJaeger* pOwner, _float dt)
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

void CStrikeJaeger_Move::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CStrikeJaeger_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CStrikeJaeger_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CStrikeJaeger_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CStrikeJaeger_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_F_L", CStrikeJaeger_Walk_F_L::Create());
	m_pSubStateMachine->Register_State("Walk_F_R", CStrikeJaeger_Walk_F_R::Create());
	m_pSubStateMachine->Register_State("Walk_R_F", CStrikeJaeger_Walk_R_F::Create());
}

void CStrikeJaeger_Move::Register_Transitions()
{
}


void CStrikeJaeger_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CStrikeJaeger_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CStrikeJaeger_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CStrikeJaeger_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CStrikeJaeger_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	case Client::CStrikeJaeger_Move::Walk_F_L:
		m_pSubStateMachine->Change_State("Walk_F_L");
		break;
	case Client::CStrikeJaeger_Move::Walk_F_R:
		m_pSubStateMachine->Change_State("Walk_F_R");
		break;
	case Client::CStrikeJaeger_Move::Walk_R_F:
		m_pSubStateMachine->Change_State("Walk_R_F");
		break;
	}
}

/*============================================================================*/
void CStrikeJaeger_Walk_Front::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_F")
		.Apply();
}

void CStrikeJaeger_Walk_Front::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_Front::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Walk_Back::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_B")
		.Apply();
}

void CStrikeJaeger_Walk_Back::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_Back::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Walk_Left::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_L")
		.Apply();
}

void CStrikeJaeger_Walk_Left::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_Left::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Walk_Right::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_R")
		.Apply();
}

void CStrikeJaeger_Walk_Right::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_Right::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Walk_F_L::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_FL_RFoot")
		.Apply();
}

void CStrikeJaeger_Walk_F_L::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_F_L::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Walk_F_R::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_FR_RFoot")
		.Apply();
}

void CStrikeJaeger_Walk_F_R::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_F_R::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Walk_R_F::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Walk_RF_LFoot")
		.Apply();
}

void CStrikeJaeger_Walk_R_F::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Walk_R_F::Exit(CStrikeJaeger* pOwner)
{
}
