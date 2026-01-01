#include "pch.h"
#include "SacrificeHandState_Idle.h"
#include "SacrificeHand.h"
#include "Helper_Func.h"

void CSacrificeHandState_Idle::Enter(CSacrificeHand* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrificeHand>::Create();

		m_pSubStateMachine->Register_State("Idle_Air",CSacrificeHandState_Idle_Air::Create());
		m_pSubStateMachine->Register_State("Idle_Ground",CSacrificeHandState_Idle_Ground::Create());

		m_pSubStateMachine->Set_DefaultState("Idle_Ground");
	}

	__super::Enter(pOwner);
}

void CSacrificeHandState_Idle::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeHandState_Idle::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Idle_Air::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringerHand_Ani_Idle_Air").Loop(true).Apply();
}

void CSacrificeHandState_Idle_Air::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Idle_Air::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Idle_Ground::Enter(CSacrificeHand* pOwner)
{
	_uint iRandIndex = Helper::Get_Random_Int(0, 1);
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (0 == iRandIndex)
		pAnimator->Change_Animation("SacrificeBringerHand_Ani_Idle_Ground01").Loop(true).Apply();
	else
		pAnimator->Change_Animation("SacrificeBringerHand_Ani_Idle_Ground02").Loop(true).Apply();
}

void CSacrificeHandState_Idle_Ground::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeHandState_Idle_Ground::Exit(CSacrificeHand* pOwner)
{
}
