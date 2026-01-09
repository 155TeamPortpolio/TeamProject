#include "pch.h"
#include "Sacrifice_Laser.h"
#include "EffectContainer.h"
#include "ObjectContainer.h"
#include "GameInstance.h"

CSacrifice_Laser::CSacrifice_Laser()
	:CGameObject()
{
}

CSacrifice_Laser::CSacrifice_Laser(const CSacrifice_Laser& rhg)
	:CGameObject(rhg)
{
}

HRESULT CSacrifice_Laser::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();

	ResourceManager()->Add_ResourcePath("laser_core2.png", "../Bin/Resources/Effect/laser_core2.png");
	ResourceManager()->Add_ResourcePath("laser_core.png", "../Bin/Resources/Effect/laser_core.png");
	ResourceManager()->Add_ResourcePath("laser_wide.png", "../Bin/Resources/Effect/laser_wide.png");
	ResourceManager()->Add_ResourcePath("laser.json", "../Bin/Resources/Effect/laser.json");

	return S_OK;
}

HRESULT CSacrifice_Laser::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pLaser = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("laser.json")
		.Build("Laser");

	auto pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Add_Child(pLaser);


	return S_OK;
}

void CSacrifice_Laser::Awake()
{
}

void CSacrifice_Laser::Priority_Update(_float dt)
{
}

void CSacrifice_Laser::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CSacrifice_Laser::Late_Update(_float dt)
{
}

CSacrifice_Laser* CSacrifice_Laser::Create()
{
	CSacrifice_Laser* instance = new CSacrifice_Laser();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrifice_Laser");
	}

	return instance;
}

CGameObject* CSacrifice_Laser::Clone(INIT_DESC* pArg)
{
	CSacrifice_Laser* instance = new CSacrifice_Laser(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrifice_Laser");
	}

	return instance;
}

void CSacrifice_Laser::Free()
{
	__super::Free();

}
