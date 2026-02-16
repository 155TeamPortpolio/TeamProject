#include "pch.h"
#include "Hand_Core.h"

//component
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"

CHand_Core::CHand_Core()
	:CEnemy()
{
}

CHand_Core::CHand_Core(const CHand_Core& rhg)
	:CEnemy(rhg)
{
}

HRESULT CHand_Core::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	Add_Component<CBoneFollower>();

	return S_OK;
}

HRESULT CHand_Core::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pRigidBody = Get_Component<CRigidBody>();
	pRigidBody->Set_Kinematic(true);

	auto pCollider = Get_Component<CCollider>();
	pCollider->Set_Trigger(false);
	pCollider->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	pCollider->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	pCollider->Set_Size(_float3(5.f, 5.f, 5.f));
	pCollider->Set_CompActive(false);

	{
		BATTLE_COLLIDER_DESC HandCore{};

		HandCore.tagName = "Hand_Core";
		HandCore.isAttachBone = false;
		HandCore.tagBone = "";
		HandCore.pOwnerAnimator3D = nullptr;
		HandCore.eAttackColliderType = COLLIDER_TYPE::SPHERE;
		HandCore.vAttackSize = _float3{ 3.f,3.f,3.f };

		if (FAILED(AttachBattleColliderObject(&HandCore, false)))
			return E_FAIL;
	}

	m_isAlive = false;

	return S_OK;
}

void CHand_Core::Awake()
{
}

void CHand_Core::Priority_Update(_float dt)
{
}

void CHand_Core::Update(_float dt)
{
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);

	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_WorldPos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CHand_Core::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CHand_Core::Render_GUI()
{
	__super::Render_GUI();
}

void CHand_Core::Active_Hand()
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
	SetBattleColliderObject("Hand_Core", CEnemy::BATTLE_COLTYPE::ATTACK, true, HitDesc);

}

void CHand_Core::Deactive_Hand()
{
	m_isAlive = false;
	m_isOnAttack = false;
	Get_Component<CCollider>()->Set_CompActive(false);
	
	SetBattleColliderObject("Hand_Core", CEnemy::BATTLE_COLTYPE::ATTACK, false);

}

CHand_Core* CHand_Core::Create()
{
	CHand_Core* instance = new CHand_Core();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CHand_Core");
	}

	return instance;
}

CGameObject* CHand_Core::Clone(INIT_DESC* pArg)
{
	CHand_Core* instance = new CHand_Core(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CHand_Core");
	}

	return instance;
}

void CHand_Core::Free()
{
	__super::Free();
}
