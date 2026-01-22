#include "pch.h"
#include "BattleEndPoint.h"
#include "GameInstance.h"

#include "Collider.h"

CBattleEndPoint::CBattleEndPoint()
	: CBattleObject()
{
}

CBattleEndPoint::CBattleEndPoint(const CBattleEndPoint& rhs)
	: CBattleObject(rhs)
{
}

HRESULT CBattleEndPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattleEndPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pColliderCom = Get_Component<CCollider>();

	pColliderCom->Set_MapToolMode(true);
	pColliderCom->Set_ColliderColor({ 0.5f, 0.2f, 1.f, 1.f });

	m_eBattleType = BATTLE_TYPE::ENDPOINT;


	return S_OK;
}

void CBattleEndPoint::Awake()
{
	__super::Awake();
}

void CBattleEndPoint::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBattleEndPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleEndPoint::Late_Update(_float dt)
{
}

CBattleEndPoint* CBattleEndPoint::Create()
{
	CBattleEndPoint* instance = new CBattleEndPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBattleEndPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBattleEndPoint::Clone(INIT_DESC* pArg)
{
	CBattleEndPoint* instance = new CBattleEndPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBattleEndPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CBattleEndPoint::Free()
{
	__super::Free();
}

