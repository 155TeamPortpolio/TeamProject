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
		if (pOwner->GetHysteriesis().fEvadeEnter >= pOwner->GetTargetingInfo().fDistance)
			iMovePatternIndex = MOVEINDEX::Walk_Back;




	}
	ChangeMovePatternFromIndex(iMovePatternIndex);

}

void CThugAssaulter_Move::Update(CThugAssaulter* pOwner, _float dt)
{
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
}

void CThugAssaulter_Walk_Evade::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Walk_Evade::Exit(CThugAssaulter* pOwner)
{
}
