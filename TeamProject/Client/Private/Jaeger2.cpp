#include "pch.h"
#include "Jaeger2.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"

CJaeger2::CJaeger2()
	:CServiceNpc()
{
}

CJaeger2::CJaeger2(const CJaeger2& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT CJaeger2::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "StrikeJaeger.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "StrikeJaeger.mat");

	return S_OK;
}

HRESULT CJaeger2::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJaeger2::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "StrikeJaeger.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "LightJaeger_Meta.json");

	m_strAnimName = "LightJaeger_Ani_";
	m_strName = L"병사";

	Get_Component<CMaterial>()->SetBlendHasAlpha(AlphaCheckLevel::Fast, "Blend");

	pAnimator->Set_Animation(Get_AnimName() + "Maincity_Idle_Normal01")
		.Loop(true)
		.Apply();

	__super::Awake();
}

void CJaeger2::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CJaeger2::Update(_float dt)
{
	__super::Update(dt);
}

void CJaeger2::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

CJaeger2* CJaeger2::Create()
{
	CJaeger2* instance = new CJaeger2();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CJaeger2");
	}

	return instance;
}

CGameObject* CJaeger2::Clone(INIT_DESC* pArg)
{
	CJaeger2* instance = new CJaeger2(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CJaeger2");
	}
	return instance;
}

void CJaeger2::Free()
{
	__super::Free();
}
