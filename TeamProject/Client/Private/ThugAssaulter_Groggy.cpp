#include "pch.h"
#include "ThugAssaulter_Groggy.h"
#include "ThugAssaulter.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Groggy::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();

		Register_States();
		Register_Transitions();

		m_pSubStateMachine->Set_DefaultState("StunStart");
	}

	__super::Enter(pOwner);
}

void CThugAssaulter_Groggy::Update(CThugAssaulter* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	if ("StunLoop" == m_pSubStateMachine->Get_CurrentStateName() &&
		0 >= pOwner->GetStatus().iGroggyValue) {
		m_pSubStateMachine->Set_Trigger("Loop_To_End");
	}
}

void CThugAssaulter_Groggy::Exit(CThugAssaulter* pOwner)
{
}

void CThugAssaulter_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStart", CThugAssaulter_Stun_Start::Create());
	m_pSubStateMachine->Register_State("StunLoop", CThugAssaulter_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CThugAssaulter_Stun_End::Create());
}

void CThugAssaulter_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStart", "StunLoop",
		CStateMachine<CThugAssaulter>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunLoop", "StunEnd",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Loop_To_End");
}


/*============================================================================*/
void CThugAssaulter_Stun_Start::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Debuff_Stun_Start")
		.Apply();
}

void CThugAssaulter_Stun_Start::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Stun_Start::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Stun_Loop::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CThugAssaulter_Stun_Loop::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Stun_Loop::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Stun_End::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Debuff_Stun_End")
		.Apply();
}

void CThugAssaulter_Stun_End::Update(CThugAssaulter* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CThugAssaulter_Stun_End::Exit(CThugAssaulter* pOwner)
{
}
