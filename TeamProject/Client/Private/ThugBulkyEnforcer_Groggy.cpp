#include "pch.h"
#include "ThugBulkyEnforcer_Groggy.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugBulkyEnforcer_Groggy::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	if (true == pOwner->Get_StateMachine()->Get_Bool("GroggyStartBack")) {
		pOwner->Get_StateMachine()->Set_Bool("GroggyStartBack", false);
		m_pSubStateMachine->Change_State("StunStartBack");
	}
	else 
		m_pSubStateMachine->Change_State("StunStartFront");

}

void CThugBulkyEnforcer_Groggy::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}

}

void CThugBulkyEnforcer_Groggy::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStartFront", CThugBulkyEnforcer_Stun_Start_Front::Create());
	m_pSubStateMachine->Register_State("StunStartBack", CThugBulkyEnforcer_Stun_Start_Back::Create());
	m_pSubStateMachine->Register_State("StunLoop", CThugBulkyEnforcer_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CThugBulkyEnforcer_Stun_End::Create());
}

void CThugBulkyEnforcer_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStartFront", "StunLoop",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunStartBack", "StunLoop",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Loop_To_End");


}

/*============================================================================*/
void CThugBulkyEnforcer_Stun_Start_Front::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Debuff_Stun_Start_Front")
		.Apply();
}

void CThugBulkyEnforcer_Stun_Start_Front::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Stun_Start_Front::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Stun_Start_Back::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Debuff_Stun_Start_Back")
		.Apply();
}

void CThugBulkyEnforcer_Stun_Start_Back::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Stun_Start_Back::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Stun_Loop::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CThugBulkyEnforcer_Stun_Loop::Update(CThugBulkyEnforcer* pOwner, _float dt)
{

}

void CThugBulkyEnforcer_Stun_Loop::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Stun_End::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Debuff_Stun_End")
		.Apply();
}

void CThugBulkyEnforcer_Stun_End::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CThugBulkyEnforcer_Stun_End::Exit(CThugBulkyEnforcer* pOwner)
{
}
