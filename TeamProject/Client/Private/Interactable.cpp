#include "pch.h"
#include "Interactable.h"
#include "GameInstance.h"

CInteractable::CInteractable()
	: CGameObject()
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CInteractable::CInteractable(const CInteractable& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteractable::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CCollider>();
	Add_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::INTERACABLE);

	return S_OK;
}

HRESULT CInteractable::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInteractable::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
