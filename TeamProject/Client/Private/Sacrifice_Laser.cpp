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

	/* Effect Asset */
	ResourceManager()->Add_ResourcePath("laser_start.json", "../Bin/Resources/Effect/Data/laser_start.json");
	ResourceManager()->Add_ResourcePath("laser4.json", "../Bin/Resources/Effect/Data/laser4.json");
	ResourceManager()->Add_ResourcePath("laser_hit_point.json", "../Bin/Resources/Effect/Data/laser_hit_point.json");

	/* Textures */
	ResourceManager()->Add_ResourcePath("laser_core2.png", "../Bin/Resources/Effect/Texture/laser_core2.png");
	ResourceManager()->Add_ResourcePath("laser_wide.png", "../Bin/Resources/Effect/Texture/laser_wide.png");
	ResourceManager()->Add_ResourcePath("Eff_Disorder_UU_23.png", "../Bin/Resources/Effect/Texture/Eff_Disorder_UU_23.png");
	ResourceManager()->Add_ResourcePath("lightning6.png", "../Bin/Resources/Effect/Texture/lightning6.png");
	ResourceManager()->Add_ResourcePath("Eff_Flare_085.png", "../Bin/Resources/Effect/Texture/Eff_Flare_085.png");
	ResourceManager()->Add_ResourcePath("Flare_UU_02.png", "../Bin/Resources/Effect/Texture/Flare_UU_02.png");
	ResourceManager()->Add_ResourcePath("Flare_UU_02.png", "../Bin/Resources/Effect/Texture/Flare_UU_02.png");

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

	auto pLaserHitPoint = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("laser_hit_point.json")
		.Build("LaserHitPoint");

	_smatrix offsetMatrix = _smatrix::Identity;
	offsetMatrix.Translation(_vector3(0.3f, 0.2f, 0.f));

	auto pBoneFollower = Get_Component<CBoneFollower>();
	pBoneFollower->Set_Offset(offsetMatrix);

	auto pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Add_Child(pLaser, true);
	pObjectContainer->Add_Child(pLaserStart,true);
	pObjectContainer->Add_Child(pLaserHitPoint, false);

	m_isAlive = false;
	
	return S_OK;
}

void CSacrifice_Laser::Awake()
{
}

void CSacrifice_Laser::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CSacrifice_Laser::Update(_float dt)
{
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);

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
		rayDesc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);
		rayDesc.vOrigin = vPosition0;
		rayDesc.vDirection = vDir;
		rayDesc.fMaxDistance = 200.f;

		if (PhysicsSystem()->Raycast(rayDesc, output))
		{
			vPosition1 = output.vPoint;
			string name = output.pHitObject->Get_InstanceName();
		}
		else
			vPosition1 = vPosition0 + vDir * 200.f;

		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = vPosition1;
	}break;
	case 1: /* Target */
	{
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();

		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = vPosition0 + m_vTargetDir * 200.f;

	}break;
	case 2: /* Look */
	{
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1{};
		_vector3 vDir = m_pTransform->Dir(STATE::RIGHT);
		vDir.y = 0.f;
		vDir.Normalize();
		vDir *= -1.f;

		PHYSICS_RAY rayDesc{};
		PHYSICS_RAY_HIT output{};
		rayDesc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);
		rayDesc.vOrigin = vPosition0;
		rayDesc.vDirection = vDir;
		rayDesc.fMaxDistance = 200.f;

		if (PhysicsSystem()->Raycast(rayDesc, output))
			vPosition1 = output.vPoint;
		else
			vPosition1 = vPosition0 + vDir * 200.f;

		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = vPosition1;
	}break;
	default:
		break;
	}

	auto pLaserHitPoint = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserHitPoint");
	pLaserHitPoint->Get_Component<CTransform>()->Set_Pos(context.vLinePoint1);
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
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CSacrifice_Laser::Render_GUI()
{
	__super::Render_GUI();

	ImGui::Text("Target Pos : %f,%f,%f", m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z);
}

void CSacrifice_Laser::ActiveLaser(_uint mode)
{
	Get_Component<CBoneFollower>()->Sync_Transform(0.f, m_pTransform);
	m_IsPendingDeactive = false;
	m_isAlive = true;
	m_iLaserMode = mode;

	if (1 == m_iLaserMode)
	{
		_vector3 vTargetPos{};
		auto& battleInfos = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
		for (auto& info : battleInfos)
		{
			if (info.isOnField)
			{
				m_vTargetPos = info.vPos;
				m_vTargetPos.y += 1.f;
				break;
			}
		}

		_vector3 vDir = m_vTargetPos - _vector3(m_pTransform->Get_WorldPos());
		vDir.y = 0.f;
		vDir.Normalize();
		m_vTargetDir = vDir;
	}

	auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CEffectContainer*>(pLaser)->Play();

	auto pLaserStart = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserStart");
	static_cast<CEffectContainer*>(pLaserStart)->Play();

	auto pLaserHitPoint = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserHitPoint");
	static_cast<CEffectContainer*>(pLaserHitPoint)->Play();
}

void CSacrifice_Laser::DeactiveLaser()
{
	m_IsPendingDeactive = true;
	m_fElapseTime = 0.f;

	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CEffectContainer*>(pEffect)->Stop();

	auto pEffect2 = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserStart");
	static_cast<CEffectContainer*>(pEffect2)->Stop();

	auto pLaserHitPoint = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserHitPoint");
	static_cast<CEffectContainer*>(pLaserHitPoint)->Stop();
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