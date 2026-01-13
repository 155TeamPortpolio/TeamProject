#include "pch.h"
#include "SacrificeState_Evade.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "EffectContainer.h"

/* Component */
#include "CharacterController.h"

/* Sub States */
#include "SacrificeState_Evade_Phase1.h"
#include "SacrificeState_Evade_Phase2.h"

void CSacrificeState_Evade::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Evade_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Evade_Phase2::Create());

		__super::Enter(pOwner);
	}

	CSacrifice::PHASE currPhase = pOwner->GetCurrPhase();
	switch (currPhase)
	{
	case CSacrifice::PHASE::PHASE1:
	{
		m_pSubStateMachine->Change_State("Phase1");
	}break;
	case CSacrifice::PHASE::PHASE2:
	{
		m_pSubStateMachine->Change_State("Phase2");
	}break;
	default:
		break;
	}

	m_IsSpawnEffect = false;
}

void CSacrificeState_Evade::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		pOwner->Idle();
	}
	else
	{
		pOwner->MoveByRootMotion(dt);
	}

	if (!m_IsSpawnEffect && m_fAnimProgress >= 0.1f)
	{
		_vector3 vPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke.json")
			.Position(vPosition)
			.Build("Smoke");
		
		auto pRockParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("rock_particle.json")
			.Position(vPosition)
			.Build("Rock");

		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pEffect, { "Test_Level","Effect_Layer" });
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pRockParticle, { "Test_Level","Effect_Layer" });

		m_IsSpawnEffect = true;
	}
}

void CSacrificeState_Evade::Exit(CSacrifice* pOwner)
{
}
