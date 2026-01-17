#include "pch.h"
#include "Portal.h"
#include "LevelMgr.h"
#include "GameInstance.h"

#include "Material.h"
#include "StaticModel.h"
CPortal::CPortal()
	: CInteractable()
{
}

CPortal::CPortal(const CPortal& rhs)
	: CInteractable(rhs)
{
}

HRESULT CPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	auto pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Default.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Default.mat");

	return S_OK;
}

HRESULT CPortal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto* pDesc = static_cast<PORTAL_DESC*>(pArg);

	m_NextLevelTag = pDesc->NextNameTag;

	return S_OK;
}

void CPortal::Awake()
{
}

void CPortal::Priority_Update(_float dt)
{
}

void CPortal::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CPortal::Late_Update(_float dt)
{
}

void CPortal::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = true;

	//상호작용버튼 만들기전 임시 닿으면실행
	Interact();
}

void CPortal::OnTriggerStay(CGameObject* pOher)
{
}

void CPortal::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = false;
}

void CPortal::Interact()
{
	if (!m_bIsInteractable)
		return;

	LevelManager()->Set_LoadingLevel("Loading_Level");
	LevelManager()->Request_ChangeLevel(m_NextLevelTag, true);
}

CPortal* CPortal::Create()
{
	CPortal* Instance = new CPortal();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CPortal::Clone(INIT_DESC* pArg)
{
	CPortal* Instance = new CPortal(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CPortal::Free()
{
	__super::Free();
}


