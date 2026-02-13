#include "pch.h"
#include "ThugBulkyEnforcer_Parried.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h" 
#include "CharacterController.h"

void CThugBulkyEnforcer_Parried::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	
	CEnemy::ATTACK_SIDE eSide = pOwner->Get_AttackSide();

	switch (eSide)
	{
	case Client::CEnemy::ATTACK_SIDE::LEFT:
		m_pSubStateMachine->Change_State("Parry_Left");
		break;
	case Client::CEnemy::ATTACK_SIDE::RIGHT:
		m_pSubStateMachine->Change_State("Parry_Right"); 
		break;
	default:
		pOwner->Idle();
		return;
	}
}

void CThugBulkyEnforcer_Parried::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if (m_fAnimProgress >= 0.99f) 
		pOwner->Idle();
}

void CThugBulkyEnforcer_Parried::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Parried::Register_States()
{
	m_pSubStateMachine->Register_State("Parry_Left", CThugBulkyEnforcer_Parry_Left::Create());
	m_pSubStateMachine->Register_State("Parry_Right", CThugBulkyEnforcer_Parry_Right::Create());

}

void CThugBulkyEnforcer_Parried::Register_Transitions()
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Parry_Left::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_Parry_Left")
		.Apply();
}

void CThugBulkyEnforcer_Parry_Left::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Parry_Left::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Parry_Right::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Hit_Parry_Right")
		.Apply();
}

void CThugBulkyEnforcer_Parry_Right::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Parry_Right::Exit(CThugBulkyEnforcer* pOwner)
{
}
