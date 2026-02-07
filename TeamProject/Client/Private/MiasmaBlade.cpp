#include "pch.h"
#include "MiasmaBlade.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "StaticModel.h"
#include "Material.h"
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "Character.h"
#include "Defiler.h"

CMiasmaBlade::CMiasmaBlade()
	: CEnemy()
{
}

CMiasmaBlade::CMiasmaBlade(const CMiasmaBlade& rhs)
	:CEnemy(rhs)
{
}

HRESULT CMiasmaBlade::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Default.model");
	Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Default.mat");
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	return S_OK;
}

HRESULT CMiasmaBlade::Initialize(INIT_DESC* pArg)
{
	auto desc = static_cast<BladeDesc*>(pArg);
	__super::Initialize(desc);
	m_pOwner = desc->pOwner;
	m_isOnAttack = true;
	m_isParryEnable = true;

	Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER) |
		ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	Get_Component<CRigidBody>()->Set_Kinematic(true);
	m_pTransform->LookAt(_vector3(desc->vTargetPos));

	return S_OK;
}

void CMiasmaBlade::Awake()
{
}

void CMiasmaBlade::Priority_Update(_float dt)
{
}

void CMiasmaBlade::Update(_float dt)
{
	m_pTransform->Translate(m_pTransform->Dir(STATE::LOOK) * 25 * dt);
	Get_Component<CCollider>()->Update(dt);
}

void CMiasmaBlade::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CMiasmaBlade::Render_GUI()
{
    __super::Render_GUI();
}

void CMiasmaBlade::OnPooledAcquire(INIT_DESC* pArg)
{
	
}

void CMiasmaBlade::OnPooledRelease()
{
	m_isOnAttack = false;
}

void CMiasmaBlade::Parried()
{
	if (m_pOwner) {
		_float4 pos= m_pOwner->Get_Position();
		m_pTransform->LookAt({ pos.x,pos.y,pos.z});
		isParried = true;
		Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::MONSTER));
		Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::PLAYER_ATTACK);
	}
}

CMiasmaBlade* CMiasmaBlade::Create()
{
	CMiasmaBlade* instance = new CMiasmaBlade();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

CGameObject* CMiasmaBlade::Clone(INIT_DESC* pArg)
{
	CMiasmaBlade* instance = new CMiasmaBlade(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CMiasmaBlade::Free()
{
	__super::Free();
}

void CMiasmaBlade::OnTriggerEnter(CGameObject* pOther)

{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
	if (!Try_Hit(pOther))
		return;

	// 데미지 주는 코드
	auto pEnemy = dynamic_cast<CCharacter*>(pOther);
	if (nullptr != pEnemy)
	{
		pEnemy->Take_Damage(DAMAGE_TYPE::NORMAL, 10);
		CameraManager()->AddImpact(1, 0);
	}
}

void CMiasmaBlade::OnTriggerStay(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
	if (!Try_Hit(pOther))
		return;

	// 데미지 주는 코드
	auto pPlayer = dynamic_cast<CCharacter*>(pOther);
	if (nullptr != pPlayer)
	{
		pPlayer->Take_Damage(DAMAGE_TYPE::NORMAL, 10);
		CameraManager()->AddImpact(1, 0);
	}
}


_bool CMiasmaBlade::Try_Hit(CGameObject* pTarget)
{
	return true;
}
