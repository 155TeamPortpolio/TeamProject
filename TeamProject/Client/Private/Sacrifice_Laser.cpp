#include "pch.h"
#include "Sacrifice_Laser.h"
#include "EffectContainer.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "PhysicsSystem.h"
#include "Character.h"

/* Component */
#include "Child.h"

CSacrifice_Laser::CSacrifice_Laser()
	:CEnemy()
{
}

CSacrifice_Laser::CSacrifice_Laser(const CSacrifice_Laser& rhg)
	:CEnemy(rhg)
{
}

HRESULT CSacrifice_Laser::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CObjectContainer>();
	Add_Component<CBoneFollower>();

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
		.Position(_float3(0.f,0.f,0.8f))
		.Build("LaserStart");

	auto pLaserHitPoint = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("laser_hit_point.json")
		.Build("LaserHitPoint");

	auto pBoneFollower = Get_Component<CBoneFollower>();

	auto pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Add_Child(pLaser, true);
	pObjectContainer->Add_Child(pLaserStart,true);
	pObjectContainer->Add_Child(pLaserHitPoint, false);

	auto pRigidBody = Get_Component<CRigidBody>();
	pRigidBody->Set_Kinematic(true);

	auto pCollider = Get_Component<CCollider>();
	pCollider->Set_Trigger(false);
	pCollider->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	pCollider->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	pCollider->Set_Size(_float3(2.f, 2.f, 2.f));
	pCollider->Set_CompActive(false);

	{
		BATTLE_COLLIDER_DESC BladeDesc{};

		BladeDesc.tagName = "Laser_Attack";
		BladeDesc.isAttachBone = false;
		BladeDesc.tagBone = "";
		BladeDesc.pOwnerAnimator3D = nullptr;
		BladeDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		BladeDesc.vAttackSize = _float3{ 2.5f,2.5f,2.5f };

		if (FAILED(AttachBattleColliderObject(&BladeDesc, false)))
			return E_FAIL;
	}

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
	if (!m_IsPendingActive)
		return;

	auto pEffectContainer = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	CEffectContainer::EFFECT_CONTAINER_CONTEXT& context = static_cast<CEffectContainer*>(pEffectContainer)->GetEffectContext();

	switch (m_iLaserMode)
	{
	case 0: /* Turn */
	{
		Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform, true);
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1{};
		_vector3 vDir = m_pTransform->Dir(STATE::LOOK);
		vDir.y = 0.f;
		vDir.Normalize();

		vPosition0 += vDir * 0.8f;
		PHYSICS_RAY rayDesc{};
		PHYSICS_RAY_HIT output{};
		rayDesc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);
		rayDesc.vOrigin = vPosition0;
		rayDesc.vDirection = vDir;
		rayDesc.fMaxDistance = 200.f;

		if (PhysicsSystem()->Raycast(rayDesc, output))
		{
			vPosition1 = output.vPoint;
		}
		else
			vPosition1 = vPosition0 + vDir * 200.f;

		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = vPosition1;

		Compute_Collider(vPosition0, vPosition1);

	}break;
	case 1: /* Target */
	{
		Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform, true);
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();

		if (!m_IsOnTarget)
			Set_TargetPosition();

		vPosition0 += m_vTargetDir * 0.8f;
		context.vLinePoint0 = vPosition0;
		context.vLinePoint1 = m_vTargetPos;

		Compute_Collider(vPosition0, m_vTargetPos);

	}break;
	case 2: /* Look */
	{
		Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1{};
		_vector3 vDir = m_pTransform->Dir(STATE::RIGHT);
		vDir.y = 0.f;
		vDir.Normalize();
		vDir *= -1.f;

		vPosition0 += vDir * 0.8f;
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

		Compute_Collider(vPosition0, vPosition1);
	}break;
	default:
		break;
	}

	auto pLaserHitPoint = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserHitPoint");
	pLaserHitPoint->Get_Component<CTransform>()->Set_Pos(context.vLinePoint1);

	if (m_IsPendingDeactive)
	{
		m_fElapseTime += dt;
		if (m_fElapseTime >= m_fDuration)
		{
			m_isAlive = false;
			m_IsPendingDeactive = false;
		}

	}

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CSacrifice_Laser::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CSacrifice_Laser::Pre_EngineUpdate(_float dt)
{
	if (!m_IsPendingActive)
	{
		m_IsPendingActive = true;
		auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
		static_cast<CEffectContainer*>(pLaser)->Play();

		auto pLaserStart = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserStart");
		static_cast<CEffectContainer*>(pLaserStart)->Play();

		auto pLaserHitPoint = Get_Component<CObjectContainer>()->Find_ObjectByName("LaserHitPoint");
		static_cast<CEffectContainer*>(pLaserHitPoint)->Play();

		return;
	}

	__super::Pre_EngineUpdate(dt);
}

void CSacrifice_Laser::Render_GUI()
{
	__super::Render_GUI();

	ImGui::Text("Target Pos : %f,%f,%f", m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z);
	ImGui::Text("Target dir : %f,%f,%f", m_vTargetDir.x, m_vTargetDir.y, m_vTargetDir.z);
}

void CSacrifice_Laser::ActiveLaser(_uint mode)
{
	HitDesc		HitDesc = {};
	HitDesc.eHitType = HIT_TYPE::ONCE;
	HitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
	HitDesc.fDamage = 10.f;
	HitDesc.fInterval = 0.f;
	HitDesc.iMaxCount = 1;
	SetBattleColliderObject("Laser_Attack", CEnemy::BATTLE_COLTYPE::ATTACK, true, HitDesc);
	Get_Component<CCollider>()->Set_CompActive(true);

	m_IsPendingActive = false;
	m_IsPendingDeactive = false;
	m_isAlive = true;
	m_IsOnTarget = false;
	m_IsHitPlayer = false;
	
	m_isOnAttack = true;
	m_iLaserMode = mode;
}

void CSacrifice_Laser::DeactiveLaser()
{
	SetBattleColliderObject("Laser_Attack", CEnemy::BATTLE_COLTYPE::ATTACK, false);
	Get_Component<CCollider>()->Set_CompActive(false);

	m_isOnAttack = false;
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

void CSacrifice_Laser::Set_TargetPosition()
{
	_vector3 vDir{ 0.f,0.f,1.f };

	_vector3 vCurrPosition = m_pTransform->Get_WorldPos();
	_vector3 vTargetPosition = BattleSystem()->GetCurCharacterHandle().Get()->Get_Component<CTransform>()->Get_WorldPos();
	_vector3 vTargetDir = vTargetPosition - vCurrPosition;
	vTargetDir.y = 0.f;

	_vector3 vLook = Get_Component<CChild>()->Get_Parent()->Get_Component<CTransform>()->Dir(STATE::LOOK);
	vLook.y = 0.f;
	vLook.Normalize();

	if (vTargetDir.Length() >= 0.01f)
	{
		vTargetDir.Normalize();

		_float dot = clamp(vLook.Dot(vTargetDir), -1.f, 1.f);
		_float crossY = vLook.x * vTargetDir.z - vLook.z * vTargetDir.x;
		_float yawDelta = atan2f(crossY, dot);
		yawDelta = clamp(yawDelta, XMConvertToRadians(-20.f), XMConvertToRadians(20.f));

		_float c = cosf(yawDelta);
		_float s = sinf(yawDelta);

		vDir.x = vLook.x * c - vLook.z * s;
		vDir.y = 0.f;
		vDir.z = vLook.x * s + vLook.z * c;
		vDir.Normalize();
	}

	m_vTargetDir = vDir;

	PHYSICS_RAY rayDesc{};
	PHYSICS_RAY_HIT output{};
	rayDesc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);
	rayDesc.vOrigin = vCurrPosition;
	rayDesc.vDirection = m_vTargetDir;
	rayDesc.fMaxDistance = 200.f;

	if (PhysicsSystem()->Raycast(rayDesc, output))
		m_vTargetPos = output.vPoint;
	else
		m_vTargetPos = vCurrPosition + 200.f * vDir;

	m_IsOnTarget = true;
}

void CSacrifice_Laser::Compute_Collider(_float3 startPos, _float3 endPos)
{
	auto pAttackCollider = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser_Attack_AttackCollider")->Get_Component<CCollider>();
	auto pCollider = Get_Component<CCollider>();
	auto pRigidBody = Get_Component<CRigidBody>();

	_vector3 vLook = endPos - startPos;
	_float vLength{};
	vLook.y = 0.f;

	vLength = vLook.Length();
	vLook.Normalize();

	_vector3 vWorldUp{ 0.f,1.f,0.f };
	m_pTransform->Set_Look(vLook);

	pCollider->Set_Center(_float3(0.f,0.f,vLength * 0.5f));
	pCollider->Set_Size(_float3(1.f, 1.f, vLength));

	pAttackCollider->Set_Center(_float3(0.f, 0.f, vLength * 0.5f));
	pAttackCollider->Set_Size(_float3(2.f, 2.f, vLength));
}
