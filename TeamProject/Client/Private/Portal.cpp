#include "pch.h"
#include "Portal.h"

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

	return S_OK;
}

HRESULT CPortal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
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


