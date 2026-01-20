#include "pch.h"
#include "SacrificeState_Born.h"
#include "StateMachine.h"
#include "Sacrifice.h"
#include "SkeletalModel.h"
#include "GameInstance.h"
#include "EffectContainer.h"

void CSacrificeState_Born::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Born_Phase1", CSacrificeState_Born_Phase1::Create());
		//m_pSubStateMachine->Register_State("Idle_Phase2", CSacrificeState_Idle_Phase2::Create());

		m_pSubStateMachine->Set_DefaultState("Born_Phase1");
	}

	__super::Enter(pOwner);
}

void CSacrificeState_Born::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CSacrificeState_Born::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Born_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringer_Ani_P1_Born")
		.Speed(1.f)
		.Loop(false)
		.Apply();

	m_IsEffectSpawn = false;
}

void CSacrificeState_Born_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	Update_Effects(pOwner);
}

void CSacrificeState_Born_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Born_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pTransform = pOwner->Get_Component<CTransform>();

	if (IsCrossAnimProgress(0.07f))
	{
		_vector3 vPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
		vPosition.y += 0.5f;

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("spawn_smoke.json")
			.Position(vPosition)
			.Build("SpawnSmoke");

		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(effect, { "Test_Level","Effect_Layer" });

		m_IsEffectSpawn = true;
	}

	/* Roar Smoke */
	if (IsCrossAnimProgress(0.38f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_roar_smoke_down.json")
			.Build("Sacrifice_Roar_Smoke_Down");

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 1.f, 0.f), worldMatrix);

		auto pEffectTransform = effect->Get_Component<CTransform>();
		pEffectTransform->Set_WorldPos(vWorldPosition);

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });
	}
}

void CSacrificeState_Born_Phase2::Enter(CSacrifice* pOwner)
{
}

void CSacrificeState_Born_Phase2::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Born_Phase2::Exit(CSacrifice* pOwner)
{
}
