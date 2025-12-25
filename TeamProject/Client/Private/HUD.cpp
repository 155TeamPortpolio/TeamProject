#include "pch.h"
#include "HUD.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

CHUD::CHUD()
{
}

CHUD::CHUD(const CHUD& rhs)
	: CUIPrefab(rhs)
{
}

HRESULT CHUD::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CHUD::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CHUD::Priority_Update(_float dt)
{
}

void CHUD::Update(_float dt)
{
}

void CHUD::Late_Update(_float dt)
{
}

CGameObject* CHUD::Create()
{
	CHUD* pInstance = new CHUD();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHUD");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHUD::Clone(INIT_DESC* pArg)
{
	CHUD* pInstance = new CHUD(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CHUD");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHUD::Free()
{
	__super::Free();
}