#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Move::Enter(CCyclops* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CCyclops>::Create();

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
			iMovePatternIndex = Helper::Get_Random_Int(2, 7);
		else
			iMovePatternIndex = MOVEINDEX::Walk_Front;
	}
	ChangeMovePatternFromIndex(iMovePatternIndex);
}

void CCyclops_Move::Update(CCyclops* pOwner, _float dt)
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

void CCyclops_Move::Exit(CCyclops* pOwner)
{
}

void CCyclops_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CCyclops_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CCyclops_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CCyclops_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CCyclops_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_F_L", CCyclops_Walk_F_L::Create());
	m_pSubStateMachine->Register_State("Walk_F_R", CCyclops_Walk_F_R::Create());
	m_pSubStateMachine->Register_State("Walk_L_F", CCyclops_Walk_L_F::Create());
}

void CCyclops_Move::Register_Transitions()
{
}


void CCyclops_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CCyclops_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CCyclops_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CCyclops_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CCyclops_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	case Client::CCyclops_Move::Walk_F_L:
		m_pSubStateMachine->Change_State("Walk_F_L");
		break;
	case Client::CCyclops_Move::Walk_F_R:
		m_pSubStateMachine->Change_State("Walk_F_R");
		break;
	case Client::CCyclops_Move::Walk_L_F:
		m_pSubStateMachine->Change_State("Walk_L_F");
		break;
	}
}

/*============================================================================*/
void CCyclops_Walk_Front::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Walk_F")
		.Apply();
}

void CCyclops_Walk_Front::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_Front::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Walk_Back::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Walk_B")
		.Apply();
}

void CCyclops_Walk_Back::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_Back::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Walk_Left::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Walk_L")
		.Apply();
}

void CCyclops_Walk_Left::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_Left::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Walk_Right::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Cyclops_Ani_Walk_R")
		.Apply();
}

void CCyclops_Walk_Right::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_Right::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Walk_F_L::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Walk_FL_LFoot")
		.Apply();
}

void CCyclops_Walk_F_L::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_F_L::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Walk_F_R::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Walk_FR_LFoot")
		.Apply();
}

void CCyclops_Walk_F_R::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_F_R::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Walk_L_F::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Walk_LF_RFoot")
		.Apply();
}

void CCyclops_Walk_L_F::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Walk_L_F::Exit(CCyclops* pOwner)
{
}
