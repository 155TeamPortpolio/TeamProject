#include "pch.h"
#include "SacrificeState_Parry_Phase2.h"
#include "Sacrifice.h"

void CSacrificeState_Parry_Phase2::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Parry_R_Phase2", CSacrificeState_Parry_R_Phase2::Create());
		m_pSubStateMachine->Register_State("Parry_L_Phase2", CSacrificeState_Parry_L_Phase2::Create());

		__super::Enter(pOwner);
	}

}

void CSacrificeState_Parry_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Parry_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Parry_R_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Hit_Parry_Right").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Parry_R_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Parry_R_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Parry_L_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Hit_Parry_Left").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Parry_L_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Parry_L_Phase2::Exit(CSacrifice* pOwner)
{
}
