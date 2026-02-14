#include "pch.h"
#include "Giant.h"
#include "Giant_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Hit::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}

	auto pStateMachine = pOwner->GetStateMachine();

	_int iDir = pStateMachine->Get_Int("Dir");
	if (-1 == iDir)
		pOwner->Idle();
	Decide_L_HitState(static_cast<DIR>(iDir), pOwner->GetTargetingInfo().fDotTarget);

}

void CGiant_Hit::Update(CGiant* pOwner, _float dt)
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

void CGiant_Hit::Exit(CGiant* pOwner)
{
}

void CGiant_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Hit_H_Front", CGiant_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CGiant_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CGiant_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CGiant_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CGiant_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CGiant_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CGiant_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CGiant_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CGiant_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CGiant_Hit_L_Front_Right::Create());
}

void CGiant_Hit::Register_Transitions()
{
}

void CGiant_Hit::Decide_L_HitState(DIR eDir, _float fDot)
{
	_bool isBack = fDot < 0.f ? true : false;

	switch (eDir)
	{
	case Client::DIR::F:
		m_pSubStateMachine->Change_State("Hit_L_Front_Up");
		break;
	case Client::DIR::FR:
		m_pSubStateMachine->Change_State("Hit_L_Front_Right");
		break;
	case Client::DIR::R:
	{
		if (isBack)
			m_pSubStateMachine->Change_State("Hit_L_Back_Right");
		else
			m_pSubStateMachine->Change_State("Hit_L_Front_Right");

		break;
	}
	case Client::DIR::BR:
		m_pSubStateMachine->Change_State("Hit_L_Back_Right");
		break;
	case Client::DIR::B:
		m_pSubStateMachine->Change_State("Hit_L_Back_Up");
		break;
	case Client::DIR::BL:
		m_pSubStateMachine->Change_State("Hit_L_Back_Left");
		break;
	case Client::DIR::L:
	{
		if (isBack)
			m_pSubStateMachine->Change_State("Hit_L_Back_Left");
		else
			m_pSubStateMachine->Change_State("Hit_L_Front_Left");
		break;
	}
	case Client::DIR::FL:
		m_pSubStateMachine->Change_State("Hit_L_Front_Left");
		break;
	}
}

/*============================================================================*/
void CGiant_Hit_H_Front::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_H_Front")
		.Apply();
}

void CGiant_Hit_H_Front::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_H_Front::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_H_Back::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_H_Back")
		.Apply();
}

void CGiant_Hit_H_Back::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_H_Back::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Back_Down::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Down")
		.Apply();
}

void CGiant_Hit_L_Back_Down::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Back_Down::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Back_Up::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Up")
		.Apply();
}

void CGiant_Hit_L_Back_Up::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Back_Up::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Back_Left::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Left")
		.Apply();
}

void CGiant_Hit_L_Back_Left::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Back_Left::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Back_Right::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Right")
		.Apply();
}

void CGiant_Hit_L_Back_Right::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Back_Right::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Front_Down::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Down")
		.Apply();
}

void CGiant_Hit_L_Front_Down::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Front_Down::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Front_Up::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Up")
		.Apply();
}

void CGiant_Hit_L_Front_Up::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Front_Up::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Front_Left::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Left")
		.Apply();
}

void CGiant_Hit_L_Front_Left::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Front_Left::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Hit_L_Front_Right::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Right")
		.Apply();
}

void CGiant_Hit_L_Front_Right::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Hit_L_Front_Right::Exit(CGiant* pOwner)
{
}
