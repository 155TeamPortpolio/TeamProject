#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Move::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	pOwner->CaptureRotateToDir(pOwner->GetTargetingInfo().vDirToTarget);

	auto pOwnerStateMachine = pOwner->GetStateMachine();

	_int iMovePatternIndex = {};

	if (-1 != pOwner->GetStateMachine()->Get_Int("MovePattern")) {
		iMovePatternIndex = pOwner->GetStateMachine()->Get_Int("MovePattern");
		pOwner->GetStateMachine()->Set_Int("MovePattern", -1);
	}
	else {
		auto hysteriesis = pOwner->GetHysteriesis();
		_float fDistance = pOwner->GetTargetingInfo().fDistance;
		if (hysteriesis.fEvadeEnter >= fDistance)
			iMovePatternIndex = MOVEINDEX::Walk_Back;
		else if (hysteriesis.fComboExit <= fDistance)
			iMovePatternIndex = MOVEINDEX::Walk_Front;
		else
			iMovePatternIndex = Helper::Get_Random_Int(3, 7);
	}
	ChangeMovePatternFromIndex(iMovePatternIndex);
}

void CThugAssaulter_Move::Update(CThugAssaulter* pOwner, _float dt)
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

void CThugAssaulter_Move::Exit(CThugAssaulter* pOwner)
{
}

void CThugAssaulter_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CThugAssaulter_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CThugAssaulter_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CThugAssaulter_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CThugAssaulter_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_FL_LFoot", CThugAssaulter_Walk_FL_LFoot::Create());
	m_pSubStateMachine->Register_State("Walk_FR_LFoot", CThugAssaulter_Walk_FR_LFoot::Create());
	m_pSubStateMachine->Register_State("Walk_LF_RFoot", CThugAssaulter_Walk_LF_RFoot::Create());
	m_pSubStateMachine->Register_State("Evade", CThugAssaulter_Walk_Evade::Create());
}

void CThugAssaulter_Move::Register_Transitions()
{
}


void CThugAssaulter_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CThugAssaulter_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CThugAssaulter_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CThugAssaulter_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CThugAssaulter_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	case Client::CThugAssaulter_Move::Walk_FL_LFoot:
		m_pSubStateMachine->Change_State("Walk_FL_LFoot");
		break;
	case Client::CThugAssaulter_Move::Walk_FR_LFoot:
		m_pSubStateMachine->Change_State("Walk_FR_LFoot");
		break;
	case Client::CThugAssaulter_Move::Walk_LF_RFoot:
		m_pSubStateMachine->Change_State("Walk_LF_RFoot");
		break;
	case Client::CThugAssaulter_Move::Evade:
		m_pSubStateMachine->Change_State("Evade");
		break;
	}
	
}

/*============================================================================*/
void CThugAssaulter_Walk_Front::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugAssaulter_Ani_Walk_F")
		.Apply();
}

void CThugAssaulter_Walk_Front::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_Front::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_Back::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugAssaulter_Ani_Walk_B")
		.Apply();
}

void CThugAssaulter_Walk_Back::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_Back::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_Left::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugAssaulter_Ani_Walk_L")
		.Apply();
}

void CThugAssaulter_Walk_Left::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_Left::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_Right::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugAssaulter_Ani_Walk_R")
		.Apply();
}

void CThugAssaulter_Walk_Right::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_Right::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_FL_LFoot::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Walk_FL_LFoot")
		.Apply();
}

void CThugAssaulter_Walk_FL_LFoot::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_FL_LFoot::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_FR_LFoot::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Walk_FR_LFoot")
		.Apply();
}

void CThugAssaulter_Walk_FR_LFoot::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_FR_LFoot::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_LF_RFoot::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Walk_LF_RFoot")
		.Apply();
}

void CThugAssaulter_Walk_LF_RFoot::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_LF_RFoot::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Walk_Evade::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Evade")
		.Apply();
}

void CThugAssaulter_Walk_Evade::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_Evade::Exit(CThugAssaulter* pOwner)
{
}
