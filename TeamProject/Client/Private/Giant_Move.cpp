#include "pch.h"
#include "Giant.h"
#include "Giant_Move.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Move::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

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

		if (fDistance <= hysteriesis.fEvadeEnter)
			iMovePatternIndex = MOVEINDEX::Walk_Back;
		else
			iMovePatternIndex = MOVEINDEX::Walk_Front;
	}
	ChangeMovePatternFromIndex(iMovePatternIndex);
}

void CGiant_Move::Update(CGiant* pOwner, _float dt)
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

void CGiant_Move::Exit(CGiant* pOwner)
{
}

void CGiant_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CGiant_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CGiant_Walk_Back::Create());
}

void CGiant_Move::Register_Transitions()
{
}


void CGiant_Move::ChangeMovePatternFromIndex(_int iMoveIndex)
{
	switch (static_cast<MOVEINDEX>(iMoveIndex))
	{
	case Client::CGiant_Move::Walk_Front:
		m_pSubStateMachine->Change_State("Walk_Front");
		break;
	case Client::CGiant_Move::Walk_Back:
		m_pSubStateMachine->Change_State("Walk_Back");
		break;
	}
}

/*============================================================================*/
void CGiant_Walk_Front::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Walk_F")
		.Apply();
}

void CGiant_Walk_Front::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Walk_Front::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Walk_Back::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Walk_B")
		.Apply();
}

void CGiant_Walk_Back::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Walk_Back::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
