#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Death.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Death::Enter(CThugPoacher* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugPoacher>::Create();

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

void CThugPoacher_Death::Update(CThugPoacher* pOwner, _float dt)
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

void CThugPoacher_Death::Exit(CThugPoacher* pOwner)
{
}

void CThugPoacher_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CThugPoacher_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CThugPoacher_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CThugPoacher_Death_Stay::Create());
}

void CThugPoacher_Death::Register_Transitions()
{
}

/*============================================================================*/
void CThugPoacher_Death_Front::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Death_Front")
		.Apply();
}

void CThugPoacher_Death_Front::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Death_Front::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Death_Back::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Death_Back")
		.Apply();
}

void CThugPoacher_Death_Back::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Death_Back::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Death_Stay::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Death_Stay")
		.Apply();
}

void CThugPoacher_Death_Stay::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Death_Stay::Exit(CThugPoacher* pOwner)
{
}
