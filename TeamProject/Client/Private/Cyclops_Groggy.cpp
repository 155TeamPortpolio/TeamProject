#include "pch.h"
#include "Cyclops.h"
#include "Cyclops_Groggy.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CCyclops_Groggy::Enter(CCyclops* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CCyclops>::Create();

		Register_States();
		Register_Transitions();

		m_pSubStateMachine->Set_DefaultState("StunStart");
	}

	__super::Enter(pOwner);
}

void CCyclops_Groggy::Update(CCyclops* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}
}

void CCyclops_Groggy::Exit(CCyclops* pOwner)
{
}

void CCyclops_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStart", CCyclops_Stun_Start::Create());
	m_pSubStateMachine->Register_State("StunLoop", CCyclops_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CCyclops_Stun_End::Create());
}

void CCyclops_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStart", "StunLoop",
		CStateMachine<CCyclops>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Loop_To_End");
}

/*============================================================================*/
void CCyclops_Stun_Start::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Debuff_Stun_Start")
		.Apply();
}

void CCyclops_Stun_Start::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Stun_Start::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Stun_Loop::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CCyclops_Stun_Loop::Update(CCyclops* pOwner, _float dt)
{
}

void CCyclops_Stun_Loop::Exit(CCyclops* pOwner)
{
}

/*============================================================================*/
void CCyclops_Stun_End::Enter(CCyclops* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Cyclops_Ani_Debuff_Stun_End")
		.Apply();
}

void CCyclops_Stun_End::Update(CCyclops* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CCyclops_Stun_End::Exit(CCyclops* pOwner)
{
}
					