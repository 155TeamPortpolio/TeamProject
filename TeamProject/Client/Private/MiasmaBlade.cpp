#include "pch.h"
#include "MiasmaBlade.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "StaticModel.h"
#include "Material.h"

#include "RigidBody.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "Character.h"
#include "CharacterController.h"
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
	Add_Component<CCharacterController>();
	return S_OK;
}

HRESULT CMiasmaBlade::Initialize(INIT_DESC* pArg)
{
	auto desc = static_cast<BladeDesc*>(pArg);
	__super::Initialize(desc);
	m_pOwner = desc->pOwner;
	m_isOnAttack = true;
	m_isParryEnable = true;
	m_vTargetPos = _vector3(desc->vTargetPos);

	Get_Component<CCharacterController>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER) | 
		ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	Get_Component<CCharacterController>()->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	Get_Component<CCharacterController>()->Set_GravityEnabled(false);

	Get_Component<CCharacterController>()->Resize(0.2f, 0.2f);
	Get_Component<CCharacterController>()->Set_RestOffset(0);
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
	m_pTransform->LookAt(m_vTargetPos);
	Get_Component<CCharacterController>()->Move_RootMotion(m_pTransform->Dir(STATE::LOOK),_quaternion().Identity, dt);
	Get_Component<CCharacterController>()->Update(dt);
}

void CMiasmaBlade::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
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
		//m_vTargetPos = {pos.x,pos.y,pos.z};
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
