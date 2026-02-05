#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Hit.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Hit::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();

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

void CStrikeJaeger_Hit::Update(CStrikeJaeger* pOwner, _float dt)
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

void CStrikeJaeger_Hit::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Knock", CStrikeJaeger_Hit_Knock::Create());
	m_pSubStateMachine->Register_State("Hit_H_Front", CStrikeJaeger_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CStrikeJaeger_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CStrikeJaeger_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CStrikeJaeger_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CStrikeJaeger_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CStrikeJaeger_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CStrikeJaeger_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CStrikeJaeger_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CStrikeJaeger_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CStrikeJaeger_Hit_L_Front_Right::Create());
}

void CStrikeJaeger_Hit::Register_Transitions()
{
}

void CStrikeJaeger_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CStrikeJaeger_Hit_Knock::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Hit_Knock")
		.Apply();
}

void CStrikeJaeger_Hit_Knock::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_Knock::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_H_Front::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_H_Front")
		.Apply();
}

void CStrikeJaeger_Hit_H_Front::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_H_Front::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_H_Back::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_H_Back")
		.Apply();
}

void CStrikeJaeger_Hit_H_Back::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_H_Back::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Back_Down::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Down")
		.Apply();
}

void CStrikeJaeger_Hit_L_Back_Down::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Back_Down::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Back_Up::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Up")
		.Apply();
}

void CStrikeJaeger_Hit_L_Back_Up::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Back_Up::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Back_Left::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Left")
		.Apply();
}

void CStrikeJaeger_Hit_L_Back_Left::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Back_Left::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Back_Right::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Back_Right")
		.Apply();
}

void CStrikeJaeger_Hit_L_Back_Right::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Back_Right::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Front_Down::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Down")
		.Apply();
}

void CStrikeJaeger_Hit_L_Front_Down::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Front_Down::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Front_Up::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Up")
		.Apply();
}

void CStrikeJaeger_Hit_L_Front_Up::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Front_Up::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Front_Left::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Left")
		.Apply();
}

void CStrikeJaeger_Hit_L_Front_Left::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Front_Left::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Hit_L_Front_Right::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Hit_L_Front_Right")
		.Apply();
}

void CStrikeJaeger_Hit_L_Front_Right::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Hit_L_Front_Right::Exit(CStrikeJaeger* pOwner)
{
}
