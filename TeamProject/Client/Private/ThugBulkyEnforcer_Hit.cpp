#include "pch.h"
#include "ThugBulkyEnforcer_Hit.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugBulkyEnforcer_Hit::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}

	auto pStateMachine = pOwner->Get_StateMachine();

	_int iDir = pStateMachine->Get_Int("Dir");
	if (-1 == iDir)
		pOwner->Idle();
	Decide_L_HitState(static_cast<DIR>(iDir), pOwner->GetTargetingInfo().fDotTarget);

}

void CThugBulkyEnforcer_Hit::Update(CThugBulkyEnforcer* pOwner, _float dt)
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

void CThugBulkyEnforcer_Hit::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Hit_H_Front", CThugBulkyEnforcer_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CThugBulkyEnforcer_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CThugBulkyEnforcer_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CThugBulkyEnforcer_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CThugBulkyEnforcer_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CThugBulkyEnforcer_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CThugBulkyEnforcer_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CThugBulkyEnforcer_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CThugBulkyEnforcer_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CThugBulkyEnforcer_Hit_L_Front_Right::Create());
}

void CThugBulkyEnforcer_Hit::Register_Transitions()
{
}

void CThugBulkyEnforcer_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CThugBulkyEnforcer_Hit_H_Front::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_H_Front")
		.Apply();
}

void CThugBulkyEnforcer_Hit_H_Front::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_H_Front::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_H_Back::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_H_Back")
		.Apply();
}

void CThugBulkyEnforcer_Hit_H_Back::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_H_Back::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Back_Down::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Back_Down")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Back_Down::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Back_Down::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Back_Up::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Back_Up")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Back_Up::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Back_Up::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Back_Left::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Back_Left")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Back_Left::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Back_Left::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Back_Right::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Back_Right")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Back_Right::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Back_Right::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Front_Down::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Front_Down")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Front_Down::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Front_Down::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Front_Up::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Front_Up")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Front_Up::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Front_Up::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Front_Left::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Front_Left")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Front_Left::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Front_Left::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Hit_L_Front_Right::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_L_Front_Right")
		.Apply();
}

void CThugBulkyEnforcer_Hit_L_Front_Right::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Hit_L_Front_Right::Exit(CThugBulkyEnforcer* pOwner)
{
}
