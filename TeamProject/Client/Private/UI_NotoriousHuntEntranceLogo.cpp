#include "pch.h"
#include "UI_NotoriousHuntEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

CUI_NotoriousHuntEntranceLogo::CUI_NotoriousHuntEntranceLogo()
	:CUI_EntranceLogo()
{
}

CUI_NotoriousHuntEntranceLogo::CUI_NotoriousHuntEntranceLogo(const CUI_NotoriousHuntEntranceLogo& rhs)
	:CUI_EntranceLogo(rhs)
{
}

HRESULT CUI_NotoriousHuntEntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>()->Link_Model("Scott_Level", "UI_NotoriousHuntLogo.model");
	Add_Component<CMaterial>()->Link_Material("Scott_Level", "UI_NotoriousHuntLogo.mat");
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CUI_NotoriousHuntEntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strName = L"악명 높은 사냥";

	return S_OK;
}

void CUI_NotoriousHuntEntranceLogo::Awake()
{
	__super::Awake();
}

void CUI_NotoriousHuntEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_NotoriousHuntEntranceLogo::Update(_float dt)
{
	__super::Update(dt);
}

void CUI_NotoriousHuntEntranceLogo::Late_Update(_float dt)
{
}

CGameObject* CUI_NotoriousHuntEntranceLogo::Create()
{
	CUI_NotoriousHuntEntranceLogo* pInstance = new CUI_NotoriousHuntEntranceLogo();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_NotoriousHuntEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NotoriousHuntEntranceLogo::Clone(INIT_DESC* pArg)
{
	CUI_NotoriousHuntEntranceLogo* pInstance = new CUI_NotoriousHuntEntranceLogo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_NotoriousHuntEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NotoriousHuntEntranceLogo::Free()
{
	__super::Free();
}