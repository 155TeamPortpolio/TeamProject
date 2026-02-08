#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Groggy.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Groggy::Enter(CMeleeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CMeleeJaeger>::Create();

		Register_States();
		Register_Transitions();

		m_pSubStateMachine->Set_DefaultState("StunStart");
	}

	__super::Enter(pOwner);
}

void CMeleeJaeger_Groggy::Update(CMeleeJaeger* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}
}

void CMeleeJaeger_Groggy::Exit(CMeleeJaeger* pOwner)
{
}

void CMeleeJaeger_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStart", CMeleeJaeger_Stun_Start::Create());
	m_pSubStateMachine->Register_State("StunLoop", CMeleeJaeger_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CMeleeJaeger_Stun_End::Create());
}

void CMeleeJaeger_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStart", "StunLoop",
		CStateMachine<CMeleeJaeger>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CMeleeJaeger>::CONDITION_TRIGGER, "Loop_To_End");
}

/*============================================================================*/
void CMeleeJaeger_Stun_Start::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Debuff_Stun_Start")
		.Apply();
}

void CMeleeJaeger_Stun_Start::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Stun_Start::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Stun_Loop::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CMeleeJaeger_Stun_Loop::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Stun_Loop::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Stun_End::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Debuff_Stun_End")
		.Apply();
}

void CMeleeJaeger_Stun_End::Update(CMeleeJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CMeleeJaeger_Stun_End::Exit(CMeleeJaeger* pOwner)
{
}
