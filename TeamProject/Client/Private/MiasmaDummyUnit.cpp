#include "pch.h"
#include "MiasmaDummyUnit.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "Texture.h"

#include "StateMachine.h"
#include "MiasmaJaegerState.h"

#include "Helper_Func.h"
#include "UIDirector.h"
#include "UI_DamageText.h"

CMiasmaDummyUnit::CMiasmaDummyUnit()
	: CEnemy()
{
}

CMiasmaDummyUnit::CMiasmaDummyUnit(const CMiasmaDummyUnit& rhg)
	:CEnemy(rhg)
{
}

HRESULT CMiasmaDummyUnit::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>()->Link_Model("Zero_Level", "MiasmaJaeger.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "MiasmaJaeger.mat");
	Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT CMiasmaDummyUnit::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model("Zero_Level", "MiasmaJaeger.model");
	pAnimator->Link_MetaData("Zero_Level", "MiasmaJaeger_Meta.json");
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_Animation("HeavyJaeger_Ani_Idle").Loop(true).Apply();
	m_vRimLightColor = _float3(0.127, 0.029, 0.070);
	m_fRimLightPower = 2.2f;
	m_fDissolveTilling = 9.f;

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");
	for (const auto& instance : materialInstances)
	{
		instance->Set_Param("NoiseTexture", { dissolveTexture->Get_SRV(),"Texture2D",0 });
		instance->Set_Param("vRimLightColor", { &m_vRimLightColor,"float3",sizeof(_float3) });
		instance->Set_Param("fRimLightPower", { &m_fRimLightPower,"float",sizeof(_float) });
		instance->Set_Param("fDissolveProgress", { &m_fDissolveProgress,"float",sizeof(_float) });
		instance->Set_Param("fDissolveTiling", { &m_fDissolveTilling,"float",sizeof(_float) });
	}
	;
	m_Dissolve.DisAppear(0.f);
	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Hide_MehsByName("Grenadier");
	return S_OK;
}

void CMiasmaDummyUnit::Awake()
{
}

void CMiasmaDummyUnit::Priority_Update(_float dt)
{
}

void CMiasmaDummyUnit::Update(_float dt)
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->Update_Animation(dt);
	Update_Dissolve(dt);
}

void CMiasmaDummyUnit::Late_Update(_float dt)
{
}

void CMiasmaDummyUnit::Update_Dissolve(_float dt)
{

	if (m_Dissolve.fDissolveElapsedTime < m_Dissolve.fDissolveDuration)
	{
		m_Dissolve.fDissolveElapsedTime += dt;
		_float t = m_Dissolve.fDissolveElapsedTime / m_Dissolve.fDissolveDuration;

		switch (m_Dissolve.eDissolveState)
		{
		case DefilerDissolve::DISAPPEAR:
		{
			m_fDissolveProgress = t;
		}break;
		case DefilerDissolve::DISSOLVE_STATE::APPEAR:
		{
			m_fDissolveProgress = 1.f - t;
		}break;
		case DefilerDissolve::DISSOLVE_STATE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		if (DefilerDissolve::DISAPPEAR == m_Dissolve.eDissolveState)
			m_fDissolveProgress = 1.01f;
		else
			m_fDissolveProgress = 0.f;
	}
}

void CMiasmaDummyUnit::Render_GUI()
{
}

void CMiasmaDummyUnit::Parried()
{
}


CMiasmaDummyUnit* CMiasmaDummyUnit::Create()
{
	CMiasmaDummyUnit* instance = new CMiasmaDummyUnit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiasmaDummyUnit");
	}

	return instance;
}

CGameObject* CMiasmaDummyUnit::Clone(INIT_DESC* pArg)
{
	CMiasmaDummyUnit* instance = new CMiasmaDummyUnit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiasmaHeavyJaeger");
	}

	return instance;
}

void CMiasmaDummyUnit::Free()
{
	__super::Free();
}
