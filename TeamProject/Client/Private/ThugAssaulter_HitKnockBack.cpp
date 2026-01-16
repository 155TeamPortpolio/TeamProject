#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_HitKnockBack.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_HitKnockBack::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();
		Register_States();
		Register_Transitions();
	 	
		__super::Enter(pOwner);
	}
}

void CThugAssaulter_HitKnockBack::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt); 
}

void CThugAssaulter_HitKnockBack::Exit(CThugAssaulter* pOwner)
{
}

void CThugAssaulter_HitKnockBack::Register_States()
{
	m_pSubStateMachine->Register_State("Front_Down", CThugAssaulter_Hit_Front_Down::Create());
	m_pSubStateMachine->Register_State("Front_Up", CThugAssaulter_Hit_Front_Up::Create());
	m_pSubStateMachine->Register_State("Front_Left", CThugAssaulter_Hit_Front_Left::Create());
	m_pSubStateMachine->Register_State("Front_Right", CThugAssaulter_Hit_Front_Right::Create());
	m_pSubStateMachine->Register_State("Back_Down", CThugAssaulter_Hit_Back_Down::Create());
	m_pSubStateMachine->Register_State("Back_Up", CThugAssaulter_Hit_Back_Up::Create());
	m_pSubStateMachine->Register_State("Back_Left", CThugAssaulter_Hit_Back_Left::Create());
	m_pSubStateMachine->Register_State("Back_Right", CThugAssaulter_Hit_Back_Right::Create());
}

void CThugAssaulter_HitKnockBack::Register_Transitions()
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Front_Down::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Down")
		.Apply();
}

void CThugAssaulter_Hit_Front_Down::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Front_Down::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Front_Up::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Up")
		.Apply();
}

void CThugAssaulter_Hit_Front_Up::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Front_Up::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Front_Left::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Left")
		.Apply();
}

void CThugAssaulter_Hit_Front_Left::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Front_Left::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Front_Right::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Front_Right")
		.Apply();
}

void CThugAssaulter_Hit_Front_Right::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Front_Right::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Back_Down::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Down")
		.Apply();
}

void CThugAssaulter_Hit_Back_Down::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Back_Down::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Back_Up::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Up")
		.Apply();
}

void CThugAssaulter_Hit_Back_Up::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Back_Up::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Back_Left::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Left")
		.Apply();
}

void CThugAssaulter_Hit_Back_Left::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Back_Left::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Hit_Back_Right::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Hit_L_Back_Right")
		.Apply();
}

void CThugAssaulter_Hit_Back_Right::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Hit_Back_Right::Exit(CThugAssaulter* pOwner)
{
}
