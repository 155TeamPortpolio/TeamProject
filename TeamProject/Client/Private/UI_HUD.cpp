#include "pch.h"
#include "UI_HUD.h"

HRESULT CUI_HUD::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HUD::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_HUD::Update(_float dt)
{
	__super::Update(dt);
}