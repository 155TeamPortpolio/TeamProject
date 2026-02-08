#include "pch.h"
#include "StrikeJaeger.h"
#include "StrikeJaeger_Groggy.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CStrikeJaeger_Groggy::Enter(CStrikeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CStrikeJaeger>::Create();

		Register_States();
		Register_Transitions();

		m_pSubStateMachine->Set_DefaultState("StunStart");
	}

	__super::Enter(pOwner);
}

void CStrikeJaeger_Groggy::Update(CStrikeJaeger* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}
}

void CStrikeJaeger_Groggy::Exit(CStrikeJaeger* pOwner)
{
}

void CStrikeJaeger_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStart", CStrikeJaeger_Stun_Start::Create());
	m_pSubStateMachine->Register_State("StunLoop", CStrikeJaeger_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CStrikeJaeger_Stun_End::Create());
}

void CStrikeJaeger_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStart", "StunLoop",
		CStateMachine<CStrikeJaeger>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CStrikeJaeger>::CONDITION_TRIGGER, "Loop_To_End");
}

/*============================================================================*/
void CStrikeJaeger_Stun_Start::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Debuff_Stun_Start")
		.Apply();
}

void CStrikeJaeger_Stun_Start::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Stun_Start::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Stun_Loop::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CStrikeJaeger_Stun_Loop::Update(CStrikeJaeger* pOwner, _float dt)
{
}

void CStrikeJaeger_Stun_Loop::Exit(CStrikeJaeger* pOwner)
{
}

/*============================================================================*/
void CStrikeJaeger_Stun_End::Enter(CStrikeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("StrikeJaeger_Ani_Debuff_Stun_End")
		.Apply();
}

void CStrikeJaeger_Stun_End::Update(CStrikeJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CStrikeJaeger_Stun_End::Exit(CStrikeJaeger* pOwner)
{
}
