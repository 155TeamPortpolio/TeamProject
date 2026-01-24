#include "pch.h"
#include "BattleMonsterPoint.h"
#include "GameInstance.h"

#include "Collider.h"

CBattleMonsterPoint::CBattleMonsterPoint()
	: CBattleObject()
{
}

CBattleMonsterPoint::CBattleMonsterPoint(const CBattleMonsterPoint& rhs)
	: CBattleObject(rhs)
{
}

HRESULT CBattleMonsterPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattleMonsterPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pColliderCom = Get_Component<CCollider>();

	pColliderCom->Set_MapToolMode(true);
	pColliderCom->Set_ColliderColor({ 0.f, 1.f, 1.f, 1.f });

	m_eBattleType = BATTLE_TYPE::MONSTER;


	return S_OK;
}

void CBattleMonsterPoint::Awake()
{
	__super::Awake();
}

void CBattleMonsterPoint::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBattleMonsterPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleMonsterPoint::Late_Update(_float dt)
{
}

CBattleMonsterPoint* CBattleMonsterPoint::Create()
{
	CBattleMonsterPoint* instance = new CBattleMonsterPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBattleMonsterPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBattleMonsterPoint::Clone(INIT_DESC* pArg)
{
	CBattleMonsterPoint* instance = new CBattleMonsterPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBattleMonsterPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CBattleMonsterPoint::Free()
{
	__super::Free();
}

