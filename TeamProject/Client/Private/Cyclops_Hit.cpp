#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Hit::Enter(CCyclops* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CCyclops>::Create();

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

void CCyclops_Hit::Update(CCyclops* pOwner, _float dt)
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

void CCyclops_Hit::Exit(CCyclops* pOwner)
{
}

void CCyclops_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Knock", CCyclops_Hit_Knock::Create());
	m_pSubStateMachine->Register_State("Hit_H_Front", CCyclops_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CCyclops_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CCyclops_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CCyclops_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CCyclops_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CCyclops_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CCyclops_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CCyclops_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CCyclops_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CCyclops_Hit_L_Front_Right::Create());
}

void CCyclops_Hit::Register_Transitions()
{
}

void CCyclops_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CCyclops_Hit_Knock::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_Knock")
		.Apply();
}

void CCyclops_Hit_Knock::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_Knock::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_H_Front::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_H_Front")
		.Apply();
}

void CCyclops_Hit_H_Front::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_H_Front::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_H_Back::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_H_Back")
		.Apply();
}

void CCyclops_Hit_H_Back::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_H_Back::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Back_Down::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Back_Down")
		.Apply();
}

void CCyclops_Hit_L_Back_Down::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Back_Down::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Back_Up::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Back_Up")
		.Apply();
}

void CCyclops_Hit_L_Back_Up::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Back_Up::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Back_Left::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Back_Left")
		.Apply();
}

void CCyclops_Hit_L_Back_Left::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Back_Left::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Back_Right::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Back_Right")
		.Apply();
}

void CCyclops_Hit_L_Back_Right::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Back_Right::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Front_Down::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Front_Down")
		.Apply();
}

void CCyclops_Hit_L_Front_Down::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Front_Down::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Front_Up::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Front_Up")
		.Apply();
}

void CCyclops_Hit_L_Front_Up::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Front_Up::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Front_Left::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Front_Left")
		.Apply();
}

void CCyclops_Hit_L_Front_Left::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Front_Left::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Hit_L_Front_Right::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Hit_L_Front_Right")
		.Apply();
}

void CCyclops_Hit_L_Front_Right::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Hit_L_Front_Right::Exit(CCyclops* pOwner)
{
}
