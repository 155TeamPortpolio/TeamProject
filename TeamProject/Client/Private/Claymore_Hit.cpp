#include "pch.h"
#include "Claymore.h"
#include "Claymore_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Hit::Enter(CClaymore* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CClaymore>::Create();

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

void CClaymore_Hit::Update(CClaymore* pOwner, _float dt)
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

void CClaymore_Hit::Exit(CClaymore* pOwner)
{
}

void CClaymore_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Knock", CClaymore_Hit_Knock::Create());
	m_pSubStateMachine->Register_State("Hit_H_Front", CClaymore_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CClaymore_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CClaymore_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CClaymore_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CClaymore_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CClaymore_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CClaymore_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CClaymore_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CClaymore_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CClaymore_Hit_L_Front_Right::Create());
}

void CClaymore_Hit::Register_Transitions()
{
}

void CClaymore_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CClaymore_Hit_Knock::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Hit_Knock")
		.Apply();
}

void CClaymore_Hit_Knock::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_Knock::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_H_Front::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_H_Front")
		.Apply();
}

void CClaymore_Hit_H_Front::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_H_Front::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_H_Back::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_H_Back")
		.Apply();
}

void CClaymore_Hit_H_Back::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_H_Back::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Back_Down::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Down")
		.Apply();
}

void CClaymore_Hit_L_Back_Down::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Back_Down::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Back_Up::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Up")
		.Apply();
}

void CClaymore_Hit_L_Back_Up::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Back_Up::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Back_Left::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Left")
		.Apply();
}

void CClaymore_Hit_L_Back_Left::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Back_Left::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Back_Right::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Right")
		.Apply();
}

void CClaymore_Hit_L_Back_Right::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Back_Right::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Front_Down::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Down")
		.Apply();
}

void CClaymore_Hit_L_Front_Down::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Front_Down::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Front_Up::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Up")
		.Apply();
}

void CClaymore_Hit_L_Front_Up::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Front_Up::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Front_Left::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Left")
		.Apply();
}

void CClaymore_Hit_L_Front_Left::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Front_Left::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Hit_L_Front_Right::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Right")
		.Apply();
}

void CClaymore_Hit_L_Front_Right::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Hit_L_Front_Right::Exit(CClaymore* pOwner)
{
}
