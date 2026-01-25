#include "pch.h"
#include "SacrificeState_ChangePhase.h"
#include "Animator3D.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "EffectContainer.h"
#include "CamDirector.h"

void CSacrificeState_ChangePhase::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_SwitchPhase").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_ChangePhase::Update(CSacrifice* pOwner, _float dt)
{
	Update_Effects(pOwner);
	Update_RimLightColor(pOwner);
}

void CSacrificeState_ChangePhase::Exit(CSacrifice* pOwner)
{
	pOwner->SetPhase(CSacrifice::PHASE::PHASE2);
	pOwner->ChangePhase_SetUp();
	pOwner->Set_Hitable(true);
}

void CSacrificeState_ChangePhase::Update_Effects(CSacrifice* pOwner)
{
	auto pTransform = pOwner->Get_Component<CTransform>();

	/* Change Phase */
	if (IsCrossAnimProgress(0.01f))
	{
		_vector3 vWorldPosition = pTransform->Get_WorldPos();
		vWorldPosition.y += 0.1f;

		auto effectDown = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_change_phase_down.json")
			.Position(vWorldPosition)
			.Build("Sacrifice_Change_Phase_Down");

		auto effectUp = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_change_phase_up.json")
			.Position(vWorldPosition)
			.Build("Sacrifice_Change_Phase_Up");

		auto effectCharge = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_change_phase_charge.json")
			.Position(vWorldPosition)
			.Build("Sacrifice_Change_Phase_Charge");

		ObjectManager()->Add_Object(effectDown, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
		ObjectManager()->Add_Object(effectUp, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
		ObjectManager()->Add_Object(effectCharge, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
	}

	/* Smoke */
	if (IsCrossAnimProgress(0.45f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();

		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Ctr_Eye6");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke.json")
			.Position(vBonePosition)
			.Build("Hit_Ground_Smoke");

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
	}

	/* Roar Smoke */
	if (IsCrossAnimProgress(0.7f))
	{
		/* Down */
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_roar_smoke_down.json")
			.Build("Sacrifice_Roar_Smoke_Down");

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 1.f, 0.f), worldMatrix);

		auto pEffectTransform = effect->Get_Component<CTransform>();
		pEffectTransform->Set_WorldPos(vWorldPosition);

		/* Up */
		auto effect2 = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_roar_smoke_up.json")
			.Build("Sacrifice_Roar_Smoke_Up");

		_vector3 vWorldPosition2 = _vector3::Transform(_vector3(0.f, 4.f, 0.f), worldMatrix);
		auto pEffectTransform2 = effect2->Get_Component<CTransform>();
		pEffectTransform2->Set_WorldPos(vWorldPosition2);

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });
		ObjectManager()->Add_Object(effect2, { pOwner->Get_Level(),"Effect_Layer" });

		CameraManager()->SetShakeType(ENUM(CamShakeType::Roar15S));
	}
}

void CSacrificeState_ChangePhase::Update_RimLightColor(CSacrifice* pOwner)
{
	if (m_fStateTime < m_fRimLightChangeDuration)
	{
		_float t = m_fStateTime / m_fRimLightChangeDuration;
		_float3 vColor = _vector3::Lerp(m_vStartColor, m_vEndColor, Math::ApplyEase(EaseType::Linear, t));

		pOwner->Set_RimLightColor(vColor);
	}
}
