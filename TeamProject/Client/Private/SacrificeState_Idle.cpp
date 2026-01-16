#include "pch.h"
#include "SacrificeState_Idle.h"
#include "Sacrifice.h"

void CSacrificeState_Idle::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Idle_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Idle_Phase2::Create());
		m_pSubStateMachine->Register_State("OverDrive", CSacrificeState_Idle_OverDrive::Create());

		__super::Enter(pOwner);
	}

	CSacrifice::PHASE currPhse = pOwner->GetCurrPhase();
	switch (currPhse)
	{
	case CSacrifice::PHASE::PHASE1:
	{
		m_pSubStateMachine->Change_State("Phase1");
	}break;
	case CSacrifice::PHASE::PHASE2:
	{
		if (pOwner->IsOverDrive() && pOwner->IsOverDriveCharged())
			m_pSubStateMachine->Change_State("OverDrive");
		else
			m_pSubStateMachine->Change_State("Phase2");
	}break;
	default:
		break;
	}
}

void CSacrificeState_Idle::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner,dt);
}

void CSacrificeState_Idle::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase1::Enter(CSacrifice* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("SacrificeBringer_Ani_P1_Idle")
		.Loop(true)
		.Speed(1.2f)
		.Apply();
}

void CSacrificeState_Idle_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Idle_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_Phase2::Enter(CSacrifice* pOwner)
{

	if (CSacrifice::DISSOLVE_STATE::DISAPPEAR == pOwner->Get_DissolveState())
	{
		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::APPEAR, 0.1f);
		pOwner->Get_Component<CAnimator3D>()->Set_Animation("SacrificeBringer_Ani_P2_Idle")
			.Loop(true)
			.Speed(1.2f)
			.Apply();
	}
	else
	{
		pOwner->Get_Component<CAnimator3D>()->Change_Animation("SacrificeBringer_Ani_P2_Idle")
			.Loop(true)
			.Speed(1.2f)
			.Apply();
	}
}

void CSacrificeState_Idle_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	pOwner->Update_Dissolve(dt);
}

void CSacrificeState_Idle_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Idle_OverDrive::Enter(CSacrifice* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack_Idle_Loop")
		.Loop(true)
		.Speed(1.2f)
		.BlendDuration(0.7f)
		.Apply();
}

void CSacrificeState_Idle_OverDrive::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Idle_OverDrive::Exit(CSacrifice* pOwner)
{
}
