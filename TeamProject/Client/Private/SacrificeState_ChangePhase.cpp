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

	/* Roar Smoke */
	if (IsCrossAnimProgress(0.7f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_roar_smoke_down.json")
			.Build("Sacrifice_Roar_Smoke_Down");

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 1.f, 0.f), worldMatrix);

		auto pEffectTransform = effect->Get_Component<CTransform>();
		pEffectTransform->Set_WorldPos(vWorldPosition);

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });

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
