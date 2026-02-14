#include "pch.h"
#include "Claymore.h"
#include "Claymore_Groggy.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CClaymore_Groggy::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

		Register_States();
		Register_Transitions();

		m_pSubStateMachine->Set_DefaultState("StunStart");
	}

	__super::Enter(pOwner);
}

void CClaymore_Groggy::Update(CGiant* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}
}

void CClaymore_Groggy::Exit(CGiant* pOwner)
{
}

void CClaymore_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStart", CClaymore_Stun_Start::Create());
	m_pSubStateMachine->Register_State("StunLoop", CClaymore_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CClaymore_Stun_End::Create());
}

void CClaymore_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStart", "StunLoop",
		CStateMachine<CGiant>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Loop_To_End");
}

/*============================================================================*/
void CClaymore_Stun_Start::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Debuff_Stun_Start")
		.Apply();
}

void CClaymore_Stun_Start::Update(CGiant* pOwner, _float dt)
{
}

void CClaymore_Stun_Start::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CClaymore_Stun_Loop::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CClaymore_Stun_Loop::Update(CGiant* pOwner, _float dt)
{
}

void CClaymore_Stun_Loop::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CClaymore_Stun_End::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Claymore_Ani_Debuff_Stun_End")
		.Apply();
}

void CClaymore_Stun_End::Update(CGiant* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CClaymore_Stun_End::Exit(CGiant* pOwner)
{
}
