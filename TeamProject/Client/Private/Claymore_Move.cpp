#include "pch.h"
#include "Claymore.h"
#include "Claymore_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Move::Enter(CClaymore* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CClaymore>::Create();

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

		iMovePatternIndex = Helper::Get_Random_Int(3, 7);
	}
	ChangeMovePatternFromIndex(iMovePatternIndex);
}

void CClaymore_Move::Update(CClaymore* pOwner, _float dt)
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

void CClaymore_Move::Exit(CClaymore* pOwner)
{
}

void CClaymore_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CClaymore_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CClaymore_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CClaymore_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CClaymore_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_F_L", CClaymore_Walk_F_L::Create());
	m_pSubStateMachine->Register_State("Walk_F_R", CClaymore_Walk_F_R::Create());
	m_pSubStateMachine->Register_State("Walk_R_F", CClaymore_Walk_R_F::Create());
}

void CClaymore_Move::Register_Transitions()
{
}


void CClaymore_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CClaymore_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CClaymore_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CClaymore_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CClaymore_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	case Client::CClaymore_Move::Walk_F_L:
		m_pSubStateMachine->Change_State("Walk_F_L");
		break;
	case Client::CClaymore_Move::Walk_F_R:
		m_pSubStateMachine->Change_State("Walk_F_R");
		break;
	case Client::CClaymore_Move::Walk_R_F:
		m_pSubStateMachine->Change_State("Walk_R_F");
		break;
	}
}

/*============================================================================*/
void CClaymore_Walk_Front::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_F")
		.Apply();
}

void CClaymore_Walk_Front::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_Front::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Walk_Back::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_B")
		.Apply();
}

void CClaymore_Walk_Back::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_Back::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Walk_Left::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_L")
		.Apply();
}

void CClaymore_Walk_Left::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_Left::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Walk_Right::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_R")
		.Apply();
}

void CClaymore_Walk_Right::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_Right::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Walk_F_L::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_F_L")
		.Apply();
}

void CClaymore_Walk_F_L::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_F_L::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Walk_F_R::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_F_R")
		.Apply();
}

void CClaymore_Walk_F_R::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_F_R::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Walk_R_F::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Walk_R_F")
		.Apply();
}

void CClaymore_Walk_R_F::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Walk_R_F::Exit(CClaymore* pOwner)
{
}
