#include "pch.h"
#include "UI_ZeroEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

#include "FieldSystem.h"

CUI_ZeroEntranceLogo::CUI_ZeroEntranceLogo()
	:CUI_EntranceLogo()
{
}

CUI_ZeroEntranceLogo::CUI_ZeroEntranceLogo(const CUI_ZeroEntranceLogo& rhs)
	:CUI_EntranceLogo(rhs)
{
}

HRESULT CUI_ZeroEntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>()->Link_Model("Scott_Level", "UI_ZeroLogo.model");
	Add_Component<CMaterial>()->Link_Material("Scott_Level", "UI_ZeroLogo.mat");
	Add_Component<CCollider>();
 
	return S_OK;
}

HRESULT CUI_ZeroEntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strName = L"제로공동";

	return S_OK;
}

void CUI_ZeroEntranceLogo::Awake()
{ 
	__super::Awake();
}

void CUI_ZeroEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_ZeroEntranceLogo::Update(_float dt)
{
	__super::Update(dt);
}

void CUI_ZeroEntranceLogo::Late_Update(_float dt)
{
}

void CUI_ZeroEntranceLogo::Interact(CGameObject* pObject)
{
	FieldSystem()->RequestEnter("Party", true);
}

CGameObject* CUI_ZeroEntranceLogo::Create()
{
	CUI_ZeroEntranceLogo* pInstance = new CUI_ZeroEntranceLogo();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_ZeroEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ZeroEntranceLogo::Clone(INIT_DESC* pArg)
{
	CUI_ZeroEntranceLogo* pInstance = new CUI_ZeroEntranceLogo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_ZeroEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ZeroEntranceLogo::Free()
{
	__super::Free();
}