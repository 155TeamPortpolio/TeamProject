#include "pch.h"
#include "SacrificeHandState_Attack_Phase2.h"
#include "SacrificeHand.h"

void CSacrificeHandState_Attack_Phase2::Enter(CSacrificeHand* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrificeHand>::Create();

		Register_States();
	}

	__super::Enter(pOwner);

}
void CSacrificeHandState_Attack_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeHandState_Attack_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_Phase2::Register_States()
{
	m_pSubStateMachine->Register_State("OverDrive_Attack01", CSacrificeState_OverDrive_Release_Attack01_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack02", CSacrificeState_OverDrive_Release_Attack02_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack03", CSacrificeState_OverDrive_Release_Attack03_Phase2::Create());
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Enter(CSacrificeHand* pOwner)
{
	pOwner->SetActive(true);
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack01").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Exit(CSacrificeHand* pOwner)
{
	pOwner->SetActive(false);
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Enter(CSacrificeHand* pOwner)
{
	pOwner->SetActive(true);
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack02").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack03").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Exit(CSacrificeHand* pOwner)
{
}
