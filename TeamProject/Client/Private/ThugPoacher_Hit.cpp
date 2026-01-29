#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Hit::Enter(CThugPoacher* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugPoacher>::Create();

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

void CThugPoacher_Hit::Update(CThugPoacher* pOwner, _float dt)
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

void CThugPoacher_Hit::Exit(CThugPoacher* pOwner)
{
}

void CThugPoacher_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Knock", CThugPoacher_Hit_Knock::Create());
	m_pSubStateMachine->Register_State("Hit_H_Front", CThugPoacher_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CThugPoacher_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CThugPoacher_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CThugPoacher_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CThugPoacher_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CThugPoacher_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CThugPoacher_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CThugPoacher_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CThugPoacher_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CThugPoacher_Hit_L_Front_Right::Create());
}

void CThugPoacher_Hit::Register_Transitions()
{
}

void CThugPoacher_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CThugPoacher_Hit_Knock::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_Knock")
		.Apply();
}

void CThugPoacher_Hit_Knock::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_Knock::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_H_Front::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_H_Front")
		.Apply();
}

void CThugPoacher_Hit_H_Front::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_H_Front::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_H_Back::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_H_Back")
		.Apply();
}

void CThugPoacher_Hit_H_Back::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_H_Back::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Back_Down::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Back_Down")
		.Apply();
}

void CThugPoacher_Hit_L_Back_Down::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Back_Down::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Back_Up::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Back_Up")
		.Apply();
}

void CThugPoacher_Hit_L_Back_Up::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Back_Up::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Back_Left::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Back_Left")
		.Apply();
}

void CThugPoacher_Hit_L_Back_Left::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Back_Left::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Back_Right::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Back_Right")
		.Apply();
}

void CThugPoacher_Hit_L_Back_Right::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Back_Right::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Front_Down::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Front_Down")
		.Apply();
}

void CThugPoacher_Hit_L_Front_Down::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Front_Down::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Front_Up::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Front_Up")
		.Apply();
}

void CThugPoacher_Hit_L_Front_Up::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Front_Up::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Front_Left::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Front_Left")
		.Apply();
}

void CThugPoacher_Hit_L_Front_Left::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Front_Left::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Hit_L_Front_Right::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Hit_L_Front_Right")
		.Apply();
}

void CThugPoacher_Hit_L_Front_Right::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Hit_L_Front_Right::Exit(CThugPoacher* pOwner)
{
}
