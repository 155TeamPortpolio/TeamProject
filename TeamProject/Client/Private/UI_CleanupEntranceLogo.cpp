#include "pch.h"
#include "UI_CleanupEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

CUI_CleanupEntranceLogo::CUI_CleanupEntranceLogo()
	:CUI_EntranceLogo()
{
}

CUI_CleanupEntranceLogo::CUI_CleanupEntranceLogo(const CUI_CleanupEntranceLogo& rhs)
	:CUI_EntranceLogo(rhs)
{
}

HRESULT CUI_CleanupEntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>()->Link_Model("Scott_Level", "UI_CleanupLogo.model");
	Add_Component<CMaterial>()->Link_Material("Scott_Level", "UI_CleanupLogo.mat");
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CUI_CleanupEntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strName = L"Å¬¸°¾÷";

	return S_OK;
}

void CUI_CleanupEntranceLogo::Awake()
{
	__super::Awake();
}

void CUI_CleanupEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_CleanupEntranceLogo::Update(_float dt)
{
	__super::Update(dt);
}

void CUI_CleanupEntranceLogo::Late_Update(_float dt)
{
}

CGameObject* CUI_CleanupEntranceLogo::Create()
{
	CUI_CleanupEntranceLogo* pInstance = new CUI_CleanupEntranceLogo();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_CleanupEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CleanupEntranceLogo::Clone(INIT_DESC* pArg)
{
	CUI_CleanupEntranceLogo* pInstance = new CUI_CleanupEntranceLogo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_CleanupEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CleanupEntranceLogo::Free()
{
	__super::Free();
}