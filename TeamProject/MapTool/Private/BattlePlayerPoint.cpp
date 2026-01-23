#include "pch.h"
#include "BattlePlayerPoint.h"
#include "GameInstance.h"

#include "Collider.h"

CBattlePlayerPoint::CBattlePlayerPoint()
	: CBattleObject()
{
}

CBattlePlayerPoint::CBattlePlayerPoint(const CBattlePlayerPoint& rhs)
	: CBattleObject(rhs)
{
}

HRESULT CBattlePlayerPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattlePlayerPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pColliderCom = Get_Component<CCollider>();

	pColliderCom->Set_MapToolMode(true);
	pColliderCom->Set_ColliderColor({ 1.f, 0.f, 1.f, 1.f });

	m_eBattleType = BATTLE_TYPE::PLAYER;

	return S_OK;
}

void CBattlePlayerPoint::Awake()
{
	__super::Awake();
}

void CBattlePlayerPoint::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBattlePlayerPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattlePlayerPoint::Late_Update(_float dt)
{
}

CBattlePlayerPoint* CBattlePlayerPoint::Create()
{
	CBattlePlayerPoint* instance = new CBattlePlayerPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBattlePlayerPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBattlePlayerPoint::Clone(INIT_DESC* pArg)
{
	CBattlePlayerPoint* instance = new CBattlePlayerPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBattlePlayerPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CBattlePlayerPoint::Free()
{
	__super::Free();
}

