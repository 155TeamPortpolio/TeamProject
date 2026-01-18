#include "pch.h"
#include "ZeroPortal.h"
#include "LevelMgr.h"
#include "GameInstance.h"

//Components
#include "Material.h"
#include "StaticModel.h"
#include "EventListener.h"

CZeroPortal::CZeroPortal()
	: CInteractable()
{
}

CZeroPortal::CZeroPortal(const CZeroPortal& rhs)
	: CInteractable(rhs)
{
}

HRESULT CZeroPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CZeroPortal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto* pDesc = static_cast<ZEROPORTAL_DESC*>(pArg);

	m_NextMapTag = pDesc->NextMapTag;

	Get_Component<CEventListener>()->Add_Listener<LevelSwitched>([&](LevelSwitched desc) 
		{
		});

	return S_OK;
}

void CZeroPortal::Awake()
{
}

void CZeroPortal::Priority_Update(_float dt)
{
}

void CZeroPortal::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
	Interact();
}

void CZeroPortal::Late_Update(_float dt)
{
}

void CZeroPortal::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = true;

	//상호작용버튼 만들기전 임시 닿으면실행

}

void CZeroPortal::OnTriggerStay(CGameObject* pOher)
{
	
}

void CZeroPortal::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = false;
}

void CZeroPortal::Interact()
{
	if (!m_bIsInteractable)
		return;
	
	if (InputDevice()->Key_Down('F')) {
		LevelManager()->Set_LoadingLevel("Loading_Level");
		LevelManager()->Request_ChangeLevel("Zero_Level", true);
	}
}

CZeroPortal* CZeroPortal::Create()
{
	CZeroPortal* Instance = new CZeroPortal();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CZeroPortal::Clone(INIT_DESC* pArg)
{
	CZeroPortal* Instance = new CZeroPortal(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CZeroPortal::Free()
{
	__super::Free();
}


