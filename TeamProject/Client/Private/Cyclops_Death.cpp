#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Death.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Death::Enter(CCyclops* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CCyclops>::Create();

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

void CCyclops_Death::Update(CCyclops* pOwner, _float dt)
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

void CCyclops_Death::Exit(CCyclops* pOwner)
{
}

void CCyclops_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CCyclops_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CCyclops_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CCyclops_Death_Stay::Create());
}

void CCyclops_Death::Register_Transitions()
{
}

/*============================================================================*/
void CCyclops_Death_Front::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Death_Front")
		.Apply();
}

void CCyclops_Death_Front::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Death_Front::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Death_Back::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Death_Back")
		.Apply();
}

void CCyclops_Death_Back::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Death_Back::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Death_Stay::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Stay")
		.Apply();
}

void CCyclops_Death_Stay::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Death_Stay::Exit(CCyclops* pOwner)
{
}
