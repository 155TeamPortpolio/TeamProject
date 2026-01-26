#include "pch.h"
#include "DefilerState_Born.h"
#include "Defiler.h"
#include "StateMachine.h"
#include "SkeletalModel.h"
#include "GameInstance.h"
#include "EffectContainer.h"
#include "CamDirector.h"

void CDefilerState_Born::Enter(CDefiler* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CDefiler>::Create();

		m_pSubStateMachine->Register_State("Born_Phase1", CDefilerState_Born_Phase1::Create());
		//m_pSubStateMachine->Register_State("Idle_Phase2", CSacrificeState_Idle_Phase2::Create());

		m_pSubStateMachine->Set_DefaultState("Born_Phase1");
	}

	__super::Enter(pOwner);
}

void CDefilerState_Born::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CDefilerState_Born::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Born_Phase1::Enter(CDefiler* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_IsoldetheDefiler_Ani_Born")
		.Speed(1.f)
		.Loop(false)
		.Apply();

}

void CDefilerState_Born_Phase1::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Born_Phase1::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Born_Phase2::Enter(CDefiler* pOwner)
{
}

void CDefilerState_Born_Phase2::Update(CDefiler* pOwner, _float dt)
{
}

void CDefilerState_Born_Phase2::Exit(CDefiler* pOwner)
{
}
