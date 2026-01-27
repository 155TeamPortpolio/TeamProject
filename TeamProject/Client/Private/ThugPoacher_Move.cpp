#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Move::Enter(CThugPoacher* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugPoacher>::Create();

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

void CThugPoacher_Move::Update(CThugPoacher* pOwner, _float dt)
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

void CThugPoacher_Move::Exit(CThugPoacher* pOwner)
{
}


void CThugPoacher_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CThugPoacher_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CThugPoacher_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CThugPoacher_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CThugPoacher_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_FL_RFoot", CThugPoacher_Walk_FL_RFoot::Create());
	m_pSubStateMachine->Register_State("Walk_FR_RFoot", CThugPoacher_Walk_FR_RFoot::Create());
	m_pSubStateMachine->Register_State("Walk_RF_LFoot", CThugPoacher_Walk_RF_LFoot::Create());
	m_pSubStateMachine->Register_State("Evade", CThugPoacher_Walk_Evade::Create());
}

void CThugPoacher_Move::Register_Transitions()
{
}


void CThugPoacher_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CThugPoacher_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CThugPoacher_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CThugPoacher_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CThugPoacher_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	case Client::CThugPoacher_Move::Walk_FL_RFoot:
		m_pSubStateMachine->Change_State("Walk_FL_RFoot");
		break;
	case Client::CThugPoacher_Move::Walk_FR_RFoot:
		m_pSubStateMachine->Change_State("Walk_FR_RFoot");
		break;
	case Client::CThugPoacher_Move::Walk_RF_LFoot:
		m_pSubStateMachine->Change_State("Walk_RF_LFoot");
		break;
	case Client::CThugPoacher_Move::Evade:
		m_pSubStateMachine->Change_State("Evade");
		break;
	}
}

/*============================================================================*/
void CThugPoacher_Walk_Front::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_F")
		.Apply();
}

void CThugPoacher_Walk_Front::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_Front::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_Back::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_B")
		.Apply();
}

void CThugPoacher_Walk_Back::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_Back::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_Left::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_L")
		.Apply();
}

void CThugPoacher_Walk_Left::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_Left::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_Right::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_R")
		.Apply();
}

void CThugPoacher_Walk_Right::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_Right::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_FL_RFoot::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_FL_RFoot")
		.Apply();
}

void CThugPoacher_Walk_FL_RFoot::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_FL_RFoot::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_FR_RFoot::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_FR_RFoot")
		.Apply();
}

void CThugPoacher_Walk_FR_RFoot::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_FR_RFoot::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_RF_LFoot::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugPoacher_Ani_Walk_RF_LFoot")
		.Apply();
}

void CThugPoacher_Walk_RF_LFoot::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_RF_LFoot::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Walk_Evade::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_ThugAssaulter_Ani_Walk_F")
		.Apply();
}

void CThugPoacher_Walk_Evade::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Walk_Evade::Exit(CThugPoacher* pOwner)
{
}
