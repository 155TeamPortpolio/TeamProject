#include "pch.h"
#include "ThugAssaulter_Hit.h"
#include "ThugAssaulter.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Hit::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}

	auto pStateMachine = pOwner->GetStateMachine();
	
	_int iDir = pStateMachine->Get_Int("Dir");
	if (-1 == iDir)
		pOwner->Idle();
	//DIR eDir = static_cast<DIR>(iDir);
	//_float fDot = pOwner->GetTargetingInfo().fDotTarget;
	Decide_L_HitState(static_cast<DIR>(iDir), pOwner->GetTargetingInfo().fDotTarget);

}

void CThugAssaulter_Hit::Update(CThugAssaulter* pOwner, _float dt)
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
		//pOwner->GetStateMachine()->Change_State(pOwner->GetStateMachine()->Get_PrevStateName());
}

void CThugAssaulter_Hit::Exit(CThugAssaulter* pOwner)
{
}

void CThugAssaulter_Hit::Register_States()
{
	m_pSubStateMachine->Register_State("Knock", CThugAssaulter_Hit_Knock::Create());
	m_pSubStateMachine->Register_State("Hit_H_Front", CThugAssaulter_Hit_H_Front::Create());
	m_pSubStateMachine->Register_State("Hit_H_Back", CThugAssaulter_Hit_H_Back::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Down", CThugAssaulter_Hit_L_Back_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Up", CThugAssaulter_Hit_L_Back_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Left", CThugAssaulter_Hit_L_Back_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Back_Right", CThugAssaulter_Hit_L_Back_Right::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Down", CThugAssaulter_Hit_L_Front_Down::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Up", CThugAssaulter_Hit_L_Front_Up::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Left", CThugAssaulter_Hit_L_Front_Left::Create());
	m_pSubStateMachine->Register_State("Hit_L_Front_Right", CThugAssaulter_Hit_L_Front_Right::Create());
}

void CThugAssaulter_Hit::Register_Transitions()
{
}

void CThugAssaulter_Hit::Decide_L_HitState(DIR eDir, _float fDot)
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
void CThugAssaulter_Hit_Knock::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_Knock")
		.Apply();
}

void CThugAssaulter_Hit_Knock::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Knock::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_H_Front::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_H_Front")
		.Apply();
}

void CThugAssaulter_Hit_H_Front::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_H_Front::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_H_Back::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_H_Back")
		.Apply();
}

void CThugAssaulter_Hit_H_Back::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_H_Back::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Back_Down::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Down")
		.Apply();
}

void CThugAssaulter_Hit_L_Back_Down::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Back_Down::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Back_Up::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Up")
		.Apply();
}

void CThugAssaulter_Hit_L_Back_Up::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Back_Up::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Back_Left::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Left")
		.Apply();
}

void CThugAssaulter_Hit_L_Back_Left::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Back_Left::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Back_Right::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Right")
		.Apply();
}

void CThugAssaulter_Hit_L_Back_Right::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Back_Right::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Front_Down::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Down")
		.Apply();
}

void CThugAssaulter_Hit_L_Front_Down::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Front_Down::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Front_Up::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Up")
		.Apply();
}

void CThugAssaulter_Hit_L_Front_Up::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Front_Up::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Front_Left::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Left")
		.Apply();
}

void CThugAssaulter_Hit_L_Front_Left::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Front_Left::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_L_Front_Right::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Right")
		.Apply();
}

void CThugAssaulter_Hit_L_Front_Right::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_L_Front_Right::Exit(CThugAssaulter* pOwner)
{
}
