#include "pch.h"
#include "CharacterParryCollider.h"

#include "GameInstance.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

#include "RigidBody.h"
#include "Collider.h"
#include "Child.h"

#include "Enemy.h"
#include "Character.h"

CCharacterParryCollider::CCharacterParryCollider()
	: CGameObject()
{
}

CCharacterParryCollider::CCharacterParryCollider(const CCharacterParryCollider& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCharacterParryCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CCharacterParryCollider::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCharacterParryCollider::Awake()
{
}

void CCharacterParryCollider::Priority_Update(_float dt)
{
	m_vPrevPos = m_pTransform->Get_Pos();
}

void CCharacterParryCollider::Update(_float dt)
{
}

void CCharacterParryCollider::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CCharacterParryCollider::Render_GUI()
{
	__super::Render_GUI();
}

void CCharacterParryCollider::OnTriggerEnter(CGameObject* pOther)
{
	ICollidable* pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable) return;
	auto pCharacter = dynamic_cast<CCharacter*>(Get_Component<CChild>()->Get_Parent());
	if (pCharacter->Is_Invincible())	return;
	
	m_ParryableTargets.insert(pOther);
}

void CCharacterParryCollider::OnTriggerStay(CGameObject* pOther)
{
	ICollidable* pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable) return;
	auto pCharacter = dynamic_cast<CCharacter*>(Get_Component<CChild>()->Get_Parent());
	if (pCharacter->Is_Invincible())	return;

	m_ParryableTargets.insert(pOther);
}

void CCharacterParryCollider::OnTriggerExit(CGameObject* pOther)
{
	ICollidable* pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable) return;

	m_ParryableTargets.erase(pOther);
}

CCharacterParryCollider* CCharacterParryCollider::Create()
{
	CCharacterParryCollider* instance = new CCharacterParryCollider();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCharacterParryCollider");
	}

	return instance;
}

CGameObject* CCharacterParryCollider::Clone(INIT_DESC* pArg)
{
	CCharacterParryCollider* instance = new CCharacterParryCollider(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CCharacterParryCollider");
	}

	return instance;
}

