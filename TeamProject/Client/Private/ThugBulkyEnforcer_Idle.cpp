#include "pch.h"
#include "ThugBulkyEnforcer_Idle.h"
#include "ThugBulkyEnforcer.h"
#include "Animator3D.h" 

void CThugBulkyEnforcer_Idle::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Idle")
		.Loop(true)
		.Apply();
	/*__super::Enter(pOwner);*/
}

void CThugBulkyEnforcer_Idle::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner,dt);
}

void CThugBulkyEnforcer_Idle::Exit(CThugBulkyEnforcer* pOwner)
{
}
