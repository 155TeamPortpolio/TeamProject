#include "pch.h"
#include "Hand_Sword.h"

//component
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"

CHand_Sword::CHand_Sword()
	:CEnemy()
{
}

CHand_Sword::CHand_Sword(const CHand_Sword& rhg)
	:CEnemy(rhg)
{
}

HRESULT CHand_Sword::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	Add_Component<CBoneFollower>();

	return S_OK;
}

HRESULT CHand_Sword::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pRigidBody = Get_Component<CRigidBody>();
	pRigidBody->Set_Kinematic(true);

	auto pCollider = Get_Component<CCollider>();
	pCollider->Set_Trigger(false);
	pCollider->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	pCollider->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	pCollider->Set_Center(_float3(19.5f, 0.f, 0.f));
	pCollider->Set_Size(_float3(34.f, 5.f, 5.f));
	pCollider->Set_CompActive(false);

	{
		BATTLE_COLLIDER_DESC HandSword{};

		HandSword.tagName = "Hand_Sword";
		HandSword.isAttachBone = false;
		HandSword.tagBone = "";
		HandSword.pOwnerAnimator3D = nullptr;
		HandSword.eAttackColliderType = COLLIDER_TYPE::BOX;
		HandSword.vCenter = _float3{ 19.5f,0.f,0.f };
		HandSword.vAttackSize = _float3{ 34.5f,4.f,4.f };

		if (FAILED(AttachBattleColliderObject(&HandSword, false)))
			return E_FAIL;
	}

	m_isAlive = false;

	return S_OK;
}

void CHand_Sword::Awake()
{
}

void CHand_Sword::Priority_Update(_float dt)
{
}

void CHand_Sword::Update(_float dt)
{
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);

	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_WorldPos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CHand_Sword::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CHand_Sword::Render_GUI()
{
	__super::Render_GUI();
}

void CHand_Sword::Active_Sword()
{
	m_isAlive = true;
	m_isOnAttack = true;
	Get_Component<CCollider>()->Set_CompActive(true);

	HitDesc		HitDesc = {};
	HitDesc.eHitType = HIT_TYPE::ONCE;
	HitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
	HitDesc.fDamage = 10.f;
	HitDesc.fInterval = 0.f;
	HitDesc.iMaxCount = 1;
	SetBattleColliderObject("Hand_Sword", CEnemy::BATTLE_COLTYPE::ATTACK, true, HitDesc);
}

void CHand_Sword::Deactive_Sword()
{
	m_isAlive = false;
	m_isOnAttack = false;
	Get_Component<CCollider>()->Set_CompActive(false);

	SetBattleColliderObject("Hand_Sword", CEnemy::BATTLE_COLTYPE::ATTACK, false);
}

CHand_Sword* CHand_Sword::Create()
{
	CHand_Sword* instance = new CHand_Sword();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CHand_Sword");
	}

	return instance;
}

CGameObject* CHand_Sword::Clone(INIT_DESC* pArg)
{
	CHand_Sword* instance = new CHand_Sword(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CHand_Sword");
	}

	return instance;
}

void CHand_Sword::Free()
{
	__super::Free();
}
