#include "pch.h"
#include "ThugBulkyEnforcer_Attack.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugBulkyEnforcer_Attack::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
		
		// 전 공격 패턴 비교 위해 0 넣기
		pOwner->AddAttackHistoryFront(0);
	}

	m_isEndAttack = false;

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	auto pStateMachine = pOwner->Get_StateMachine();
	if (nullptr == pStateMachine) 
		return;

	// Attack확인용
	_int iAttackPatternIndex = pStateMachine->Get_Int("AttackPattern");
	if (0 != iAttackPatternIndex) {
		pStateMachine->Set_Int("AttackPattern", 0);
		BuildPattern(blackboard, iAttackPatternIndex);
	}
	else {
		if (false == DecideAttackPattern(pOwner)) {
			pOwner->Idle();
			return;
		}
	}

	//auto pStateMachine = pOwner->Get_StateMachine();
	//if (nullptr == pStateMachine)
	//	return;
	//_int iAttackPatternIndex = pStateMachine->Get_Int("AttackPattern");
	//if (0 != iAttackPatternIndex) {
	//	pStateMachine->Set_Int("AttackPattern", 0);
	//	BuildPattern(blackboard, iAttackPatternIndex);
	//}
	//else {
	//	auto RandomNums = Pick3RandomIndex();
	//
	//	for (size_t i = 0; i < RandomNums.size(); i++)
	//	{
	//		if (i < RandomNums.size() - 1)
	//			BuildPattern(blackboard, RandomNums[i], true);
	//		else
	//			BuildPattern(blackboard, RandomNums[i], false);
	//	}
	//}
	
	if (false != blackboard.stateQueue.empty()) {
		pOwner->Idle();
		return;
	}
	blackboard.isRequestNext = true;

}

void CThugBulkyEnforcer_Attack::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	if (true == blackboard.isRequestNext) {
		blackboard.isRequestNext = false;
		blackboard.isChainOpen = false;
	
		if (!blackboard.stateQueue.empty()) {
			string nextStateTag = blackboard.stateQueue.front();
			blackboard.stateQueue.pop_front();
			
			// 공격 테이블의 마지막 패턴일 때
			//if (blackboard.stateQueue.empty()) 
			//	blackboard.isEnd = true;
	
			blackboard.currentStateTag = nextStateTag;
			m_pSubStateMachine->Change_State(nextStateTag);
		}
		pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);
	}
	
	if (true == blackboard.isChainOpen && false == blackboard.isRequestNext) {
		blackboard.currentStateTag = "";
		pOwner->Get_StateMachine()->Set_Bool("FinishAttack", true);
		blackboard.isEnd = false;
		pOwner->Idle();
	}

}

void CThugBulkyEnforcer_Attack::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CThugBulkyEnforcer_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CThugBulkyEnforcer_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CThugBulkyEnforcer_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack04", CThugBulkyEnforcer_Attack4::Create());
	m_pSubStateMachine->Register_State("Attack05_01", CThugBulkyEnforcer_Attack5_1::Create());
	m_pSubStateMachine->Register_State("Attack05_02", CThugBulkyEnforcer_Attack5_2::Create());
	m_pSubStateMachine->Register_State("AttackSideStep_L", CThugBulkyEnforcer_AttackSideStep_L::Create());
	m_pSubStateMachine->Register_State("AttackSideStep_R", CThugBulkyEnforcer_AttackSideStep_R::Create());
	m_pSubStateMachine->Register_State("AttackEvade", CThugBulkyEnforcer_AttackEvade::Create());
}

void CThugBulkyEnforcer_Attack::Register_Transitions()
{
}

void CThugBulkyEnforcer_Attack::BuildPattern(ATTACK_BLACK_BOARD& blackBoard, _int iPatternIndex, _bool isAdditionalMoveState)
{
	
	switch (iPatternIndex)
	{  
	case 1:
	{
		blackBoard.stateQueue.push_back("Attack01");
	}break;
	case 2:
	{
		blackBoard.stateQueue.push_back("Attack02");
		//blackBoard.stateQueue.push_back("AttackEvade");

	}break;
	case 3:
	{	
		blackBoard.stateQueue.push_back("Attack03");
	}break;   
	case 4:
	{
		blackBoard.stateQueue.push_back("Attack04");

	}break;
	case 5:
	{
		//blackBoard.stateQueue.push_back("AttackSideStep_R");
		blackBoard.stateQueue.push_back("Attack05_01");

		break;
	}
	case 6:
	{
		//blackBoard.stateQueue.push_back("AttackSideStep_L");
		blackBoard.stateQueue.push_back("Attack05_02");
		break;
	}
	//case 7:
	//{
	//	blackBoard.stateQueue.push_back("AttackSideStep_L");
	//	break;
	//}
	//case 8:
	//{
	//	blackBoard.stateQueue.push_back("AttackSideStep_R");
	//	break;
	//}
	default:
		break;
	}

	//if (true == isAdditionalMoveState) {
	//	_int iMoveIndex = Helper::Get_Random_Int(0, 3);
	//
	//	switch (iMoveIndex)
	//	{
	//	case 0:
	//	case 1:
	//		break;
	//	case 2:
	//	{
	//		blackBoard.stateQueue.push_back("AttackSideStep_L");
	//		break;
	//	}
	//	case 3:
	//	{
	//		blackBoard.stateQueue.push_back("AttackSideStep_R");
	//		break;
	//	}
	//	}
	//}

}

array<_int, 3> CThugBulkyEnforcer_Attack::Pick3RandomIndex()
{
	array<_int, 6> nums{ 1,2,3,4,5,6 };

	shuffle(nums.begin(), nums.end(), Helper::Get_RNG());

	return { nums[0], nums[1] ,nums[2] };
}

_bool CThugBulkyEnforcer_Attack::DecideAttackPattern(CThugBulkyEnforcer* pOwner)
{
	const TARGETING_INFO tInfo = pOwner->GetTargetingInfo();
	const HYSTERIESIS tHysteriesis = pOwner->GetHysteriesis();
	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();

	const _float fDistanceToPlayer = tInfo.fDistance;
	_int iAttackIndex = {};

	if (fDistanceToPlayer <= tHysteriesis.fComboEnter) {
		iAttackIndex = Helper::Get_Random_Int(1, 5);

		while (iAttackIndex == pOwner->GetAttackHistoryFront())
			iAttackIndex = Helper::Get_Random_Int(1, 5);

		switch (iAttackIndex) {
		case 1:
		{
			blackboard.stateQueue.push_back("Attack01");
			//m_pSubStateMachine->Change_State("Attack01");
			break;
		}
		case 2:
		{
			blackboard.stateQueue.push_back("Attack02");
			//m_pSubStateMachine->Change_State("Attack02");
			break;
		}
		case 3:
		{
			blackboard.stateQueue.push_back("Attack04");
			//m_pSubStateMachine->Change_State("Attack04");
			break;
		}
		case 4:
		{
			blackboard.stateQueue.push_back("Attack05_01");
			//m_pSubStateMachine->Change_State("Attack05_01");
			break;
		}
		case 5:
		{
			blackboard.stateQueue.push_back("Attack05_02");
			//m_pSubStateMachine->Change_State("Attack05_02");
			break;
		}
		}
	}
	else if (fDistanceToPlayer <= tHysteriesis.fChaseExit)
	{
		iAttackIndex = Helper::Get_Random_Int(6, 8);

		while (iAttackIndex == pOwner->GetAttackHistoryFront())
			iAttackIndex = Helper::Get_Random_Int(6, 8);


		switch (iAttackIndex) {
		case 6:
		{
			blackboard.stateQueue.push_back("Attack03");
			//m_pSubStateMachine->Change_State("Attack03");
			break;
		}
		case 7:
		{
			blackboard.stateQueue.push_back("AttackSideStep_R");
			blackboard.stateQueue.push_back("Attack05_01");
			//m_pSubStateMachine->Change_State("Attack03");
			break;
		}
		case 8:
		{
			blackboard.stateQueue.push_back("AttackSideStep_L");
			blackboard.stateQueue.push_back("Attack05_02");
			//m_pSubStateMachine->Change_State("Attack03");
			break;
		}
		}
	}
	else
		return false;

	pOwner->AddAttackHistoryFront(iAttackIndex);
	return true;
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack1::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_01")
		.Apply();
}

void CThugBulkyEnforcer_Attack1::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	// 애니메이션 진행도에 따라 바꾸거나, 시간이 지나면 바뀜

	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	if ((false == blackboard.isEnd && m_fAnimProgress >= 0.47f) ||
		(true == blackboard.isEnd && m_fAnimProgress >= 0.99f)) // 0.70f
	{
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_Attack1::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack2::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_02")
		.Apply();
}

void CThugBulkyEnforcer_Attack2::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	//if (m_fAnimProgress >= 0.65f)
	if ((false == blackboard.isEnd && m_fAnimProgress >= 0.65f) ||
		(true == blackboard.isEnd && m_fAnimProgress >= 0.99f))
	{
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_Attack2::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack3::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_03")
		.Apply();
}

void CThugBulkyEnforcer_Attack3::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	//if (m_fAnimProgress >= 0.45f)
	if ((false == blackboard.isEnd && m_fAnimProgress >= 0.45f) ||
		(true == blackboard.isEnd && m_fAnimProgress >= 0.99f)) {
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_Attack3::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack4::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_04")
		.Apply();
}

void CThugBulkyEnforcer_Attack4::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	//if (m_fAnimProgress >= 0.59f)
	if ((false == blackboard.isEnd && m_fAnimProgress >= 0.59f) ||
		(true == blackboard.isEnd && m_fAnimProgress >= 0.99f)) {
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_Attack4::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack5_1::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_05_01")
		.Apply();
}

void CThugBulkyEnforcer_Attack5_1::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	//if (m_fAnimProgress >= 0.47f)
	if ((false == blackboard.isEnd && m_fAnimProgress >= 0.47f) ||
		(true == blackboard.isEnd && m_fAnimProgress >= 0.99f)) {
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_Attack5_1::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack5_2::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_05_02")
		.Apply();
}

void CThugBulkyEnforcer_Attack5_2::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	//if (m_fAnimProgress >= 0.5f)
	if ((false == blackboard.isEnd && m_fAnimProgress >= 0.65f) ||
		(true == blackboard.isEnd && m_fAnimProgress >= 0.99f)) {
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_Attack5_2::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_AttackSideStep_L::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_SideStep_L")
		.Apply();
}

void CThugBulkyEnforcer_AttackSideStep_L::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.18f)
	{
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_AttackSideStep_L::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_AttackSideStep_R::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_SideStep_R")
		.Apply();
}

void CThugBulkyEnforcer_AttackSideStep_R::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	pOwner->CaptureRotateDir(pOwner->GetTargetingInfo().vDirToTarget, 10.f);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.18f)
	{
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_AttackSideStep_R::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_AttackEvade::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Evade")
		.Apply();
}

void CThugBulkyEnforcer_AttackEvade::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.24f)
	{
		blackboard.isChainOpen = true;
		if (!blackboard.stateQueue.empty())
			blackboard.isRequestNext = true;
	}
}

void CThugBulkyEnforcer_AttackEvade::Exit(CThugBulkyEnforcer* pOwner)
{
}
