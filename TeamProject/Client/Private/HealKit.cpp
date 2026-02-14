#include "pch.h"
#include "HealKit.h"
#include "LevelMgr.h"
#include "GameInstance.h"
#include "Material.h"
#include "StaticModel.h"
#include "ObjectContainer.h"
#include "Character.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

CHealKit::CHealKit()
	: CInteractable()
{
}

CHealKit::CHealKit(const CHealKit& rhs)
	: CInteractable(rhs)
{
}

HRESULT CHealKit::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	
	return S_OK;
}

HRESULT CHealKit::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	auto* pDesc = static_cast<HEALKIT_DESC*>(pArg);

	m_eItemType = pDesc->eItemType;

	if (m_eItemType == ITEMTYPE::END)
		return E_FAIL;
	
	Get_Component<CStaticModel>()->Link_Model("Scott_Level", "Device_Prop_ItemStand_02.model");
	Get_Component<CMaterial>()->Link_Material("Scott_Level", "Device_Prop_ItemStand_02.mat");

	return S_OK;
}

void CHealKit::Awake()
{
	__super::Awake();
}

void CHealKit::Priority_Update(_float dt)
{
	if (m_bActiveItem)
		return;

	m_fElapsedTime += dt;

	if (m_fResetTimer <= m_fElapsedTime) {
		m_bActiveItem = true;
		m_fElapsedTime = 0.f;
	}

}

void CHealKit::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CHealKit::Late_Update(_float dt)
{
}

void CHealKit::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	Interact(pOther);
}

void CHealKit::OnTriggerStay(CGameObject* pOher)
{
	
}

void CHealKit::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
}

void CHealKit::Interact(CGameObject* pObject)
{
	if (!m_bActiveItem) return;

	if (CCharacter* pCharactor = dynamic_cast<CCharacter*>(pObject))
	{
		switch (m_eItemType)
		{
		case Client::CHealKit::ITEMTYPE::HP:		Recovery_Health();	break;
		case Client::CHealKit::ITEMTYPE::ENERGY:	Recovery_Energy();	break;
		case Client::CHealKit::ITEMTYPE::END:							break;	
		default:														break;
		}
	}
}

OBJECT_HANDLE CHealKit::Get_InteractHandle()
{
	return Get_Handle();
}

void CHealKit::Recovery_Health()
{
	BattleSystem()->GetBattlePlayer();
}

void CHealKit::Recovery_Energy()
{
	BattleSystem()->GetBattlePlayer();
}

CHealKit* CHealKit::Create()
{
	CHealKit* Instance = new CHealKit();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CHealKit::Clone(INIT_DESC* pArg)
{
	CHealKit* Instance = new CHealKit(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CHealKit::Free()
{
	__super::Free();
}


