#include "pch.h"
#include "Claymore.h"
#include "Claymore_Death.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Death::Enter(CClaymore* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CClaymore>::Create();

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

void CClaymore_Death::Update(CClaymore* pOwner, _float dt)
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

void CClaymore_Death::Exit(CClaymore* pOwner)
{
}

void CClaymore_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CClaymore_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CClaymore_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CClaymore_Death_Stay::Create());
}

void CClaymore_Death::Register_Transitions()
{
}

/*============================================================================*/
void CClaymore_Death_Front::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Front")
		.Apply();
}

void CClaymore_Death_Front::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Death_Front::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Death_Back::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Back")
		.Apply();
}

void CClaymore_Death_Back::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Death_Back::Exit(CClaymore* pOwner)
{
}

/*============================================================================*/
void CClaymore_Death_Stay::Enter(CClaymore* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Claymore_Ani_Death_Stay")
		.Apply();
}

void CClaymore_Death_Stay::Update(CClaymore* pOwner, _float dt)
{
}

void CClaymore_Death_Stay::Exit(CClaymore* pOwner)
{
}
