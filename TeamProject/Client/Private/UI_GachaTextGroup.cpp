#include "pch.h"
#include "UI_GachaTextGroup.h"

void CUI_GachaTextGroup::Show(GachaGrade eGrade)
{
}

HRESULT CUI_GachaTextGroup::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GachaTextGroup::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_GachaTextGroup::Update(_float dt)
{
}

CGameObject* CUI_GachaTextGroup::Create()
{
	CUI_GachaTextGroup* pInstance = new CUI_GachaTextGroup;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_GachaTextGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_GachaTextGroup::Clone(INIT_DESC* pArg)
{
	CUI_GachaTextGroup* pInstance = new CUI_GachaTextGroup(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_GachaTextGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}