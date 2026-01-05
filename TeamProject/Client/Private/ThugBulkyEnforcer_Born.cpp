#include "pch.h"
#include "ThugBulkyEnforcer_Born.h"
#include "ThugBulkyEnforcer.h"
#include "Animator3D.h" 

void CThugBulkyEnforcer_Born::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Idle")
		.Apply();
}

void CThugBulkyEnforcer_Born::Update(CThugBulkyEnforcer* pOwner, _float dt)
{

}

void CThugBulkyEnforcer_Born::Exit(CThugBulkyEnforcer* pOwner)
{
}
