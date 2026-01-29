#include "pch.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Groggy.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugPoacher_Groggy::Enter(CThugPoacher* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugPoacher>::Create();

		Register_States();
		Register_Transitions();

		m_pSubStateMachine->Set_DefaultState("StunStart");
	}

	__super::Enter(pOwner);
}

void CThugPoacher_Groggy::Update(CThugPoacher* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}
}

void CThugPoacher_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStart", CThugPoacher_Stun_Start::Create());
	m_pSubStateMachine->Register_State("StunLoop", CThugPoacher_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CThugPoacher_Stun_End::Create());
}

void CThugPoacher_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStart", "StunLoop",
		CStateMachine<CThugPoacher>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Loop_To_End");
}

void CThugPoacher_Groggy::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Stun_Start::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Debuff_Stun_Start")
		.Apply();
}

void CThugPoacher_Stun_Start::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Stun_Start::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Stun_Loop::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CThugPoacher_Stun_Loop::Update(CThugPoacher* pOwner, _float dt)
{
}

void CThugPoacher_Stun_Loop::Exit(CThugPoacher* pOwner)
{
}

/*============================================================================*/
void CThugPoacher_Stun_End::Enter(CThugPoacher* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugPoacher_Ani_Debuff_Stun_End")
		.Apply();
}

void CThugPoacher_Stun_End::Update(CThugPoacher* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CThugPoacher_Stun_End::Exit(CThugPoacher* pOwner)
{
}
