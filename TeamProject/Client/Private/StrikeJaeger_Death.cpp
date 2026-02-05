#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Death.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Death::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	if (true == pOwner->GetStateMachine()->Get_Bool("DeathBack")) {
		pOwner->GetStateMachine()->Set_Bool("DeathBack", false);
		m_pSubStateMachine->Change_State("DeathBack");
	}
	else
		m_pSubStateMachine->Change_State("DeathFront");
}

void CStrikeJaeger_Death::Update(CStrikeJaeger* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	if (m_fAnimProgress > 0.99f)
		pOwner->Death();
}

void CStrikeJaeger_Death::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CStrikeJaeger_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CStrikeJaeger_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CStrikeJaeger_Death_Stay::Create());
}

void CStrikeJaeger_Death::Register_Transitions()
{
}

/*============================================================================*/
void CStrikeJaeger_Death_Front::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Front")
		.Apply();
}

void CStrikeJaeger_Death_Front::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Death_Front::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Death_Back::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Back")
		.Apply();
}

void CStrikeJaeger_Death_Back::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Death_Back::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Death_Stay::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Stay")
		.Apply();
}

void CStrikeJaeger_Death_Stay::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Death_Stay::Exit(CStrikeJaeger* pOwner)
{
}
