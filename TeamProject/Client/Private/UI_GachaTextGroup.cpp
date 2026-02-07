#include "pch.h"
#include "UI_GachaTextGroup.h"

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