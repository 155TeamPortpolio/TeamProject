#include "pch.h"
#include "UI_DeadlyAssaultEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

CUI_DeadlyAssaultEntranceLogo::CUI_DeadlyAssaultEntranceLogo()
	:CUI_EntranceLogo()
{
}

CUI_DeadlyAssaultEntranceLogo::CUI_DeadlyAssaultEntranceLogo(const CUI_DeadlyAssaultEntranceLogo& rhs)
	:CUI_EntranceLogo(rhs)
{
}

HRESULT CUI_DeadlyAssaultEntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>()->Link_Model("Scott_Level", "UI_DeadlyAssaultLogo.model");
	Add_Component<CMaterial>()->Link_Material("Scott_Level", "UI_DeadlyAssaultLogo.mat");
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CUI_DeadlyAssaultEntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strName = L"위험한 강습전";

	return S_OK;
}

void CUI_DeadlyAssaultEntranceLogo::Awake()
{
	__super::Awake();
}

void CUI_DeadlyAssaultEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_DeadlyAssaultEntranceLogo::Update(_float dt)
{
	__super::Update(dt);
}

void CUI_DeadlyAssaultEntranceLogo::Late_Update(_float dt)
{
}

CGameObject* CUI_DeadlyAssaultEntranceLogo::Create()
{
	CUI_DeadlyAssaultEntranceLogo* pInstance = new CUI_DeadlyAssaultEntranceLogo();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_DeadlyAssaultEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_DeadlyAssaultEntranceLogo::Clone(INIT_DESC* pArg)
{
	CUI_DeadlyAssaultEntranceLogo* pInstance = new CUI_DeadlyAssaultEntranceLogo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_DeadlyAssaultEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_DeadlyAssaultEntranceLogo::Free()
{
	__super::Free();
}