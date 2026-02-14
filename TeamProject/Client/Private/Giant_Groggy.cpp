#include "pch.h"
#include "Giant.h"
#include "Giant_Groggy.h"
#include "Helper_Func.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Groggy::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

		Register_States();
		Register_Transitions();
	
		__super::Enter(pOwner);
	}
	
	if (pOwner->GetTargetingInfo().fDotTarget >= 0.f)		// 내적 값이 양수 == 앞쪽에 가까움 
		m_pSubStateMachine->Change_State("StunStartFront");
	else                                                    // 내적 값이 음수 == 뒤쪽
		m_pSubStateMachine->Change_State("StunStartBack");
}

void CGiant_Groggy::Update(CGiant* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CGiant_Groggy::Exit(CGiant* pOwner)
{
}

void CGiant_Groggy::Register_States()
{
	m_pSubStateMachine->Register_State("StunStartFront", CGiant_Stun_Start_Front::Create());
	m_pSubStateMachine->Register_State("StunStartBack", CGiant_Stun_Start_Back::Create());
	m_pSubStateMachine->Register_State("StunLoop", CGiant_Stun_Loop::Create());
	m_pSubStateMachine->Register_State("StunEnd", CGiant_Stun_End::Create());
}

void CGiant_Groggy::Register_Transitions()
{
	m_pSubStateMachine->Register_Transition("StunStartFront", "StunLoop",
		CStateMachine<CGiant>::CONDITION_ANIMATION_END);
	m_pSubStateMachine->Register_Transition("StunStartBack", "StunLoop",
		CStateMachine<CGiant>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CGiant_Stun_Start_Front::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Giant_Ani_Debuff_Stun_Start_Front")
		.Apply();
}

void CGiant_Stun_Start_Front::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Stun_Start_Front::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Stun_Start_Back::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Giant_Ani_Debuff_Stun_Start_Back")
		.Apply();
}

void CGiant_Stun_Start_Back::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Stun_Start_Back::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Stun_Loop::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Giant_Ani_Debuff_Stun_Loop")
		.Loop(true)
		.Apply();
}

void CGiant_Stun_Loop::Update(CGiant* pOwner, _float dt)
{
	if (0 >= pOwner->GetStatus().iGroggyValue)
		m_pOwnerStateMachine->Change_State("StunEnd");
}

void CGiant_Stun_Loop::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Stun_End::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_Giant_Ani_Debuff_Stun_End")
		.Apply();
}

void CGiant_Stun_End::Update(CGiant* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Idle();
}

void CGiant_Stun_End::Exit(CGiant* pOwner)
{
}
