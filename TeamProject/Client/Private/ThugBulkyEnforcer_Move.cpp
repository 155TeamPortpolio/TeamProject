#include "pch.h"
#include "ThugBulkyEnforcer_Move.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h" 
#include "CharacterController.h"

void CThugBulkyEnforcer_Move::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}

	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);

	auto pOwnerStateMachine = pOwner->Get_StateMachine();

	_int iMovePatternIndex = -1;

	if (-1 != pOwner->Get_StateMachine()->Get_Int("MovePattern")) {
		iMovePatternIndex = pOwner->Get_StateMachine()->Get_Int("MovePattern");
		pOwner->Get_StateMachine()->Set_Int("MovePattern", -1);
	}
	else if (true == pOwnerStateMachine->Get_Bool("Sidestep")) {
		auto& info = pOwner->GetTargetingInfo();
		_vector vSelfRight = XMVector3Normalize(pOwner->Get_Component<CTransform>()->Dir(Engine::STATE::RIGHT));
		_vector vDirToTarget = XMVector3Normalize(XMLoadFloat3(&info.vDirToTarget));
		_float fDot = XMVectorGetX(XMVector3Dot(vSelfRight, vDirToTarget));

		if (fDot >= 0.f)
			iMovePatternIndex = MOVEINDEX::SideStep_L;
		else
			iMovePatternIndex = MOVEINDEX::SideStep_R;

		pOwnerStateMachine->Set_Bool("Sidestep", false);
	}
	else if (true == pOwnerStateMachine->Get_Bool("Evade")) {
		iMovePatternIndex = MOVEINDEX::Evade;
		pOwnerStateMachine->Set_Bool("Evade", false);
	}
	else {
		iMovePatternIndex = Helper::Get_Random_Int(3, 7);
	}

	ChangeMovePatternFromIndex(iMovePatternIndex);

	//iMovePatternIndex = pOwner->Get_StateMachine()->Get_Int("MovePattern");
	//if (0 != iMovePatternIndex) {
	//	ChangeMovePatternFromIndex(iMovePatternIndex);
	//	pOwner->Get_StateMachine()->Set_Int("MovePattern", -1);
	//}
	
	//ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();

	//if (true == pOwner->Get_StateMachine()->Get_Bool("FinishAttack")) {
	//	RandomWalk(blackboard);
	//	pOwner->Get_StateMachine()->Set_Bool("FinishAttack", false);
	//}

	//if (false != blackboard.stateQueue.empty()) {
	//	pOwner->Idle();
	//	return;
	//}

	//blackboard.isRequestNext = true;

}

void CThugBulkyEnforcer_Move::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if (m_fAnimProgress >= 0.99f) {
		pOwner->Idle();
	}

	//ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	//if (true == blackboard.isRequestNext) {
	//	blackboard.isRequestNext = false;
	//	blackboard.isChainOpen = false;
	//
	//	if (!blackboard.stateQueue.empty()) {
	//		string nextStateTag = blackboard.stateQueue.front();
	//		blackboard.stateQueue.pop_front();
	//
	//		blackboard.currentStateTag = nextStateTag;
	//		m_pSubStateMachine->Change_State(nextStateTag);
	//	}
	//}
	//
	//if (true == blackboard.isChainOpen && false == blackboard.isRequestNext) {
	//	blackboard.currentStateTag = "";
	//	pOwner->Idle();
	//}
}

void CThugBulkyEnforcer_Move::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CThugBulkyEnforcer_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CThugBulkyEnforcer_Walk_Left::Create()); 
	m_pSubStateMachine->Register_State("Walk_Right", CThugBulkyEnforcer_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CThugBulkyEnforcer_Walk_Back::Create());
	m_pSubStateMachine->Register_State("SideStep_L", CThugBulkyEnforcer_SideStep_L::Create());
	m_pSubStateMachine->Register_State("SideStep_R", CThugBulkyEnforcer_SideStep_R::Create());
	m_pSubStateMachine->Register_State("Evade", CThugBulkyEnforcer_Evade::Create());
	m_pSubStateMachine->Register_State("Walk_RF_LFoot", CThugBulkyEnforcer_Walk_RF_LFoot::Create());
	m_pSubStateMachine->Register_State("Walk_FR_RFoot", CThugBulkyEnforcer_Walk_FR_RFoot::Create());
	m_pSubStateMachine->Register_State("Walk_LF_RFoot", CThugBulkyEnforcer_Walk_FL_RFoot::Create());
}

void CThugBulkyEnforcer_Move::Register_Transitions()
{
}

void CThugBulkyEnforcer_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CThugBulkyEnforcer_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CThugBulkyEnforcer_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	case Client::CThugBulkyEnforcer_Move::Walk_Left:
		m_pSubStateMachine->Change_State("Walk_Left");
		break;
	case Client::CThugBulkyEnforcer_Move::Walk_Right:
		m_pSubStateMachine->Change_State("Walk_Right");
		break;
	case Client::CThugBulkyEnforcer_Move::Walk_RF_LFoot:
		m_pSubStateMachine->Change_State("Walk_RF_LFoot");
		break;
	case Client::CThugBulkyEnforcer_Move::Walk_FR_RFoot:
		m_pSubStateMachine->Change_State("Walk_FR_RFoot");
		break;
	case Client::CThugBulkyEnforcer_Move::Walk_LF_RFoot:
		m_pSubStateMachine->Change_State("Walk_LF_RFoot");
		break;
	case Client::CThugBulkyEnforcer_Move::SideStep_L:
		m_pSubStateMachine->Change_State("SideStep_L");
		break;
	case Client::CThugBulkyEnforcer_Move::SideStep_R:
		m_pSubStateMachine->Change_State("SideStep_R");
		break;
	case Client::CThugBulkyEnforcer_Move::Evade:
		m_pSubStateMachine->Change_State("Evade");
		break;
	}
}

void CThugBulkyEnforcer_Move::RandomWalk(ATTACK_BLACK_BOARD& blackBoard, _int iWalkIndex)
{
	_int Index = {};

	if (0 == iWalkIndex)
		Index = Helper::Get_Random_Int(1, 5);
	else
		Index = iWalkIndex;

	switch (Index)
	{
	case 1:
	{
		blackBoard.stateQueue.push_back("Walk_Left");
		break;
	}
	case 2:
	{
		blackBoard.stateQueue.push_back("Walk_Right");
		blackBoard.stateQueue.push_back("Walk_Front");
		blackBoard.stateQueue.push_back("Walk_Back");
		break;
	}
	case 3:
	{
		blackBoard.stateQueue.push_back("Walk_RF_LFoot");
		break;
	}
	case 4:
	{
		blackBoard.stateQueue.push_back("Walk_FR_RFoot");
		break;
	}
	case 5:
	{
		blackBoard.stateQueue.push_back("Walk_LF_RFoot");
		break;
	}
	case 6:
	{
		blackBoard.stateQueue.push_back("SideStep_L");
		break;
	}
	case 7:
	{
		blackBoard.stateQueue.push_back("SideStep_R");
		break;
	}
	case 8:
	{
		blackBoard.stateQueue.push_back("Evade");
		break;
	}
	}
}

_int CThugBulkyEnforcer_Move::DecideSidestep(CThugBulkyEnforcer* pOwner)
{
	return _int();
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Front::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_F")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Front::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Walk_Front::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Left::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_L")
		//.BlendDuration(0.2f) // ¿Ö Æ¢Áö?
		.Apply();
}

void CThugBulkyEnforcer_Walk_Left::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Walk_Left::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Right::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_R")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Right::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Walk_Right::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Back::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_B")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Back::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Walk_Back::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_SideStep_L::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_SideStep_L")
		.Apply();
}

void CThugBulkyEnforcer_SideStep_L::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);

	if (true == pOwner->Get_StateMachine()->Get_Bool("ShortlyMove") &&
		m_fAnimProgress >= 0.18f) {
		pOwner->Get_StateMachine()->Set_Bool("ShortlyMove", false);
		pOwner->Idle();
	}
}

void CThugBulkyEnforcer_SideStep_L::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_SideStep_R::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_SideStep_R")
		.Apply();
}

void CThugBulkyEnforcer_SideStep_R::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);

	if (true == pOwner->Get_StateMachine()->Get_Bool("ShortlyMove") &&
		m_fAnimProgress >= 0.18f) {
		pOwner->Get_StateMachine()->Set_Bool("ShortlyMove", false);
		pOwner->Idle();
	}
}

void CThugBulkyEnforcer_SideStep_R::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Evade::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Evade")
		.Apply(); 
}

void CThugBulkyEnforcer_Evade::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Evade::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_RF_LFoot::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_RF_LFoot")
		.Apply();
}

void CThugBulkyEnforcer_Walk_RF_LFoot::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Walk_RF_LFoot::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_FR_RFoot::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_FR_RFoot")
		.Apply();
}

void CThugBulkyEnforcer_Walk_FR_RFoot::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

}

void CThugBulkyEnforcer_Walk_FR_RFoot::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_FL_RFoot::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_FL_RFoot")
		.Apply();
}

void CThugBulkyEnforcer_Walk_FL_RFoot::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

}

void CThugBulkyEnforcer_Walk_FL_RFoot::Exit(CThugBulkyEnforcer* pOwner)
{
}


