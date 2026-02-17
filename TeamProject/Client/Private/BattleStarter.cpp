#include "pch.h"
#include "BattleStarter.h"
#include "LevelMgr.h"
#include "GameInstance.h"

#include "Material.h"
#include "StaticModel.h"
#include "Zero_Level.h"
#include "Stage.h"

CBattleStarter::CBattleStarter()
	: CInteractable()
{
}

CBattleStarter::CBattleStarter(const CBattleStarter& rhs)
	: CInteractable(rhs)
{
}

HRESULT CBattleStarter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleStarter::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBattleStarter::Awake()
{
	Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::COMMON);
	m_bIsInteractable = true;
}

void CBattleStarter::Priority_Update(_float dt)
{
}

void CBattleStarter::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleStarter::Late_Update(_float dt)
{
}

void CBattleStarter::OnTriggerEnter(CGameObject* pOther)
{
}

void CBattleStarter::OnTriggerStay(CGameObject* pOher)
{
	
}

void CBattleStarter::OnTriggerExit(CGameObject* pOther)
{
	if (m_bIsInteractable) {
		auto pCollidable = pOther->Get_Component<ICollidable>();
		if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
			return;

		m_bIsInteractable = false;

		CZero_Level* pZero_Level = dynamic_cast<CZero_Level*>(CGameInstance::GetInstance()->Get_CurrentLevel());
		pZero_Level->Get_StageContext().pNowStage->Change_StageState(CStage::StageState::BattleStart);
	}
}

CBattleStarter* CBattleStarter::Create()
{
	CBattleStarter* pInstance = new CBattleStarter();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CBattleStarter::Clone(INIT_DESC* pArg)
{
	CBattleStarter* pInstance = new CBattleStarter(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CBattleStarter::Free()
{
	__super::Free();
}


