#include "pch.h"
#include "DefilerState_Born.h"
#include "Defiler.h"
#include "StateMachine.h"
#include "SkeletalModel.h"

void CDefilerState_Born::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_IsoldetheDefiler_Ani_Born")
		.Speed(1.f)
		.Loop(false)
		.Apply();

}

void CDefilerState_Born::Update(CDefiler* pOwner, _float dt)
{
	m_ElapsedTime += dt;
	if (!m_bBorn&&dt>0.3f) {
		pOwner->ControlBGM();
		m_bBorn = true;
	}

	__super::Update(pOwner, dt);
}

void CDefilerState_Born::Exit(CDefiler* pOwner)
{
}
