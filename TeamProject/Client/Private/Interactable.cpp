#include "pch.h"
#include "Interactable.h"

CInteractable::CInteractable()
	: CGameObject()
{
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
	Get_Component<CCollider>()->Set_Trigger(true);

	return S_OK;
}

HRESULT CInteractable::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

void CInteractable::Free()
{
	__super::Free();
}
