#include "pch.h"
#include "SacrificeHandState_Idle.h"
#include "SacrificeHand.h"
#include "Helper_Func.h"

void CSacrificeHandState_Idle::Enter(CSacrificeHand* pOwner)
{
	pOwner->SetVisable(false);

	__super::Enter(pOwner);
}

void CSacrificeHandState_Idle::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeHandState_Idle::Exit(CSacrificeHand* pOwner)
{
}