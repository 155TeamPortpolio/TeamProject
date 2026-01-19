#include "pch.h"
#include "SacrificeState_ChangePhase.h"
#include "Animator3D.h"
#include "Sacrifice.h"

void CSacrificeState_ChangePhase::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_SwitchPhase").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_ChangePhase::Update(CSacrifice* pOwner, _float dt)
{

}

void CSacrificeState_ChangePhase::Exit(CSacrifice* pOwner)
{
	pOwner->SetPhase(CSacrifice::PHASE::PHASE2);
	pOwner->ChangePhase_SetUp();
	pOwner->Set_Hitable(true);
}
