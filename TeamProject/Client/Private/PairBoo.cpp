#include "pch.h"
#include "PairBoo.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

CPairBoo::CPairBoo()
    :CNpc()
{
}

CPairBoo::CPairBoo(const CPairBoo& rhs)
    :CNpc(rhs)
{
}

HRESULT CPairBoo::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("Sumoboo.model", "../Bin/Resources/MainCity/NPC/ElectricBoo/Sumoboo/Sumoboo.model");
	pResource->Add_ResourcePath("Sumoboo.mat", "../Bin/Resources/MainCity/NPC/ElectricBoo/Sumoboo/Sumoboo.mat");
	pResource->Add_ResourcePath("NPC_Bangboo_Sumoboo_Meta.json", "../Bin/Resources/MainCity/NPC/ElectricBoo/Sumoboo/NPC_Bangboo_Sumoboo_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Sumoboo.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Sumoboo.mat");

	pMaterial->SetBlendHasAlpha(AlphaCheckLevel::Precise, "Blend");
	return S_OK;
}

HRESULT CPairBoo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPairBoo::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Sumoboo.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Bangboo_Sumoboo_Meta.json");

	m_strAnimName = "NPC_Bangboo_Sumoboo_Ani_HSC_";
	m_strName = L"충전부";

	pAnimator->Set_Animation(Get_AnimName() + "Idle")
		.Loop(true)
		.Apply();

	Add_NameIndicator();
	__super::Awake();
}

void CPairBoo::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CPairBoo::Update(_float dt)
{
	__super::Update(dt);
	Get_Component<CCharacterController>()->Update(dt);
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CPairBoo::Late_Update(_float dt)
{
	__super::Late_Update(dt);
	Get_Component<CCharacterController>()->Late_Update(dt);
}

CPairBoo* CPairBoo::Create()
{
	CPairBoo* instance = new CPairBoo();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CPairBoo");
	}

	return instance;
}

CGameObject* CPairBoo::Clone(INIT_DESC* pArg)
{
	CPairBoo* instance = new CPairBoo(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CPairBoo");
	}
	return instance;
}

void CPairBoo::Free()
{
	__super::Free();
}
