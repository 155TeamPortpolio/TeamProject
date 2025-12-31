#include "pch.h"
#include "SacrificeState_Attack_Phase2.h"
#include "Sacrifice.h"

void CSacrificeState_Attack_Phase2::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();
		Register_Transitions();
	}
}

void CSacrificeState_Attack_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase2::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01_Phase2",CSacrificeState_Attack_01_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase2",CSacrificeState_Attack_02_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase2",CSacrificeState_Attack_03_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack04_Phase2",CSacrificeState_Attack_04_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase2",CSacrificeState_Attack_05_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack05_1_Phase2",CSacrificeState_Attack_05_1_Phase2::Create());
}

void CSacrificeState_Attack_Phase2::Register_Transitions()
{
}

void CSacrificeState_Attack_01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_01").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_01_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_01_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_02_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_02").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_02_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_03_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_03").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_03_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_04_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack04").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_04_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_04_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_05").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_05_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_05_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_1_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_05_1").Loop(true).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_05_1_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_05_1_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Charge_U_Start").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Charge_U_Loop").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Charge_U_End").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Exit(CSacrifice* pOwner)
{
}
