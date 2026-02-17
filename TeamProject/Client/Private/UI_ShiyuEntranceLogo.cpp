#include "pch.h"
#include "UI_ShiyuEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

CUI_ShiyuEntranceLogo::CUI_ShiyuEntranceLogo()
	:CUI_EntranceLogo()
{
}

CUI_ShiyuEntranceLogo::CUI_ShiyuEntranceLogo(const CUI_ShiyuEntranceLogo& rhs)
	:CUI_EntranceLogo(rhs)
{
}

HRESULT CUI_ShiyuEntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>()->Link_Model("Scott_Level", "UI_ShiyuLogo.model");
	Add_Component<CMaterial>()->Link_Material("Scott_Level", "UI_ShiyuLogo.mat");
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CUI_ShiyuEntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strName = L"시유 방어전";

	return S_OK;
}

void CUI_ShiyuEntranceLogo::Awake()
{
	__super::Awake();
}

void CUI_ShiyuEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_ShiyuEntranceLogo::Update(_float dt)
{
	__super::Update(dt);
}

void CUI_ShiyuEntranceLogo::Late_Update(_float dt)
{
}

CGameObject* CUI_ShiyuEntranceLogo::Create()
{
	CUI_ShiyuEntranceLogo* pInstance = new CUI_ShiyuEntranceLogo();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_ShiyuEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ShiyuEntranceLogo::Clone(INIT_DESC* pArg)
{
	CUI_ShiyuEntranceLogo* pInstance = new CUI_ShiyuEntranceLogo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_ShiyuEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ShiyuEntranceLogo::Free()
{
	__super::Free();
}