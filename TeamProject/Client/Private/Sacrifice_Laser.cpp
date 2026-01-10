#include "pch.h"
#include "Sacrifice_Laser.h"
#include "EffectContainer.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"
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
	Add_Component<CBoneFollower>();

	ResourceManager()->Add_ResourcePath("laser_core2.png", "../Bin/Resources/Effect/Texture/laser_core2.png");
	ResourceManager()->Add_ResourcePath("laser_wide.png", "../Bin/Resources/Effect/Texture/laser_wide.png");
	ResourceManager()->Add_ResourcePath("Eff_Disorder_UU_23.png", "../Bin/Resources/Effect/Texture/Eff_Disorder_UU_23.png");
	ResourceManager()->Add_ResourcePath("lightning6.png", "../Bin/Resources/Effect/Texture/lightning6.png");

	ResourceManager()->Add_ResourcePath("laser_start.json", "../Bin/Resources/Effect/Data/laser_start.json");
	ResourceManager()->Add_ResourcePath("laser2.json", "../Bin/Resources/Effect/Data/laser2.json");

	return S_OK;
}

HRESULT CSacrifice_Laser::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pLaser = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("laser2.json")
		.Build("Laser");

	//auto pLaserStart = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
	//	.Asset("laser_start.json")
	//	.Build("LaserStart");
	//pObjectContainer->Add_Child(pLaserStart);

	_smatrix offsetMatrix = _smatrix::Identity;
	offsetMatrix.Translation(_vector3(1.f, 0.f, 0.f));

	auto pBoneFollower = Get_Component<CBoneFollower>();
	pBoneFollower->Set_Offset(offsetMatrix);

	auto pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Add_Child(pLaser);

	m_isAlive = false;
	
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
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	if (m_IsPendingDeactive)
	{
		m_fElapseTime += dt;
		if (m_fElapseTime >= m_fDuration)
		{
			m_isAlive = false;
			m_IsPendingDeactive = false;
		}
	}
}

void CSacrifice_Laser::Late_Update(_float dt)
{
}

void CSacrifice_Laser::ActiveLaser()
{
	m_IsPendingDeactive = false;
	m_isAlive = true;

	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CEffectContainer*>(pEffect)->Play();
}

void CSacrifice_Laser::DeactiveLaser()
{
	m_IsPendingDeactive = true;
	m_fElapseTime = 0.f;

	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CEffectContainer*>(pEffect)->Stop();
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
