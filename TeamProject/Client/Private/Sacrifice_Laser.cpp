#include "pch.h"
#include "Sacrifice_Laser.h"
#include "EffectContainer.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "PhysicsSystem.h"

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
	ResourceManager()->Add_ResourcePath("laser3.json", "../Bin/Resources/Effect/Data/laser3.json");

	return S_OK;
}

HRESULT CSacrifice_Laser::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pLaser = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("laser3.json")
		.Build("Laser");

	auto pLaserStart = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("laser_start.json")
		.Build("LaserStart");

	_smatrix offsetMatrix = _smatrix::Identity;
	offsetMatrix.Translation(_vector3(1.f, 0.f, 0.f));

	auto pBoneFollower = Get_Component<CBoneFollower>();
	pBoneFollower->Set_Offset(offsetMatrix);

	auto pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Add_Child(pLaser, true);
	pObjectContainer->Add_Child(pLaserStart,true);

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
	auto pEffectContainer = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	CEffectContainer::EFFECT_CONTAINER_CONTEXT& context = static_cast<CEffectContainer*>(pEffectContainer)->GetEffectContext();

	switch (m_iLaserMode)
	{
	case 0: /* Turn */
	{
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1{};
		_vector3 vDir = m_pTransform->Dir(STATE::RIGHT);
		vDir.y = 0.f;
		vDir.Normalize();

		PHYSICS_RAY rayDesc{};
		PHYSICS_RAY_HIT output{};
		rayDesc.vOrigin = vPosition0;
		rayDesc.vDirection = vDir;
		rayDesc.fMaxDistance = 200.f;

		if (PhysicsSystem()->Raycast(rayDesc, output))
			vPosition1 = output.vPoint;
		else
			vPosition1 = vPosition1 + vDir * 200.f;

		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = vPosition1;
	}break;
	case 1: /* Target */
	{
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1{};
		_vector3 vTargetPosition{};
		_vector3 vDir{};

		auto& battleInfos = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
		for (auto& info : battleInfos)
		{
			if (info.isOnField)
			{
				vTargetPosition = info.vPos;
				vTargetPosition.y += 1.f;

				vDir = vTargetPosition - vPosition0;
				vDir.Normalize();
				break;
			}
		}

		/*PHYSICS_RAY rayDesc{};
		PHYSICS_RAY_HIT output{};
		rayDesc.vOrigin = vPosition0;
		rayDesc.vDirection = vDir;
		rayDesc.fMaxDistance = 200.f;

		if (PhysicsSystem()->Raycast(rayDesc, output))
			vPosition1 = output.vPoint;
		else
			vPosition1 = vPosition1 + vDir * 200.f;*/

		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = vPosition0 + vDir * 200.f;

	}break;
	default:
		break;
	}

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
	for(auto& child : Get_Children());
}

void CSacrifice_Laser::ActiveLaser(_uint mode)
{
	m_IsPendingDeactive = false;
	m_isAlive = true;
	m_iLaserMode = mode;

	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CEffectContainer*>(pEffect)->Play();

	auto pEffect2 = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserStart");
	static_cast<CEffectContainer*>(pEffect2)->Play();
}

void CSacrifice_Laser::DeactiveLaser()
{
	m_IsPendingDeactive = true;
	m_fElapseTime = 0.f;

	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CEffectContainer*>(pEffect)->Stop();

	auto pEffect2 = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserStart");
	static_cast<CEffectContainer*>(pEffect2)->Stop();
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
