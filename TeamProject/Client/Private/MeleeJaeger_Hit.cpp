#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Hit::Enter(CMeleeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CMeleeJaeger>::Create();

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

void CMeleeJaeger_Hit::Update(CMeleeJaeger* pOwner, _float dt)
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

void CMeleeJaeger_Hit::Exit(CMeleeJaeger* pOwner)
{
}

void CMeleeJaeger_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Knock", CMeleeJaeger_Hit_Knock::Create());
	m_pSubStateMachine->Register_State("Hit_H_Front", CMeleeJaeger_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CMeleeJaeger_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CMeleeJaeger_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CMeleeJaeger_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CMeleeJaeger_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CMeleeJaeger_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CMeleeJaeger_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CMeleeJaeger_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CMeleeJaeger_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CMeleeJaeger_Hit_L_Front_Right::Create());
}

void CMeleeJaeger_Hit::Register_Transitions()
{
}

void CMeleeJaeger_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CMeleeJaeger_Hit_Knock::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_Knock")
		.Apply();
}

void CMeleeJaeger_Hit_Knock::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_Knock::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_H_Front::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_H_Front")
		.Apply();
}

void CMeleeJaeger_Hit_H_Front::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_H_Front::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_H_Back::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_H_Back")
		.Apply();
}

void CMeleeJaeger_Hit_H_Back::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_H_Back::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Back_Down::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Back_Down")
		.Apply();
}

void CMeleeJaeger_Hit_L_Back_Down::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Back_Down::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Back_Up::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Back_Up")
		.Apply();
}

void CMeleeJaeger_Hit_L_Back_Up::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Back_Up::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Back_Left::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Back_Left")
		.Apply();
}

void CMeleeJaeger_Hit_L_Back_Left::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Back_Left::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Back_Right::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Back_Right")
		.Apply();
}

void CMeleeJaeger_Hit_L_Back_Right::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Back_Right::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Front_Down::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Front_Down")
		.Apply();
}

void CMeleeJaeger_Hit_L_Front_Down::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Front_Down::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Front_Up::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Front_Up")
		.Apply();
}

void CMeleeJaeger_Hit_L_Front_Up::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Front_Up::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Front_Left::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Front_Left")
		.Apply();
}

void CMeleeJaeger_Hit_L_Front_Left::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Front_Left::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Hit_L_Front_Right::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Hit_L_Front_Right")
		.Apply();
}

void CMeleeJaeger_Hit_L_Front_Right::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Hit_L_Front_Right::Exit(CMeleeJaeger* pOwner)
{
}
