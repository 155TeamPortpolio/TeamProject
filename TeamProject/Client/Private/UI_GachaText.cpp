#include "pch.h"
#include "UI_GachaText.h"

#include "StaticModel.h"
#include "Material.h"

void CUI_GachaText::Change_Model(const string& strKey)
{
	if (strKey.empty())
		return;

	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", strKey + ".model");
	pMaterial->Link_Material("Gacha_Level", strKey + ".mat");
}

HRESULT CUI_GachaText::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CUI_GachaText::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "GachaStage_UI_EngineB_01.model");
	pMaterial->Link_Material("Gacha_Level", "GachaStage_UI_EngineB_01.mat");

	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
	pModel->ShadowCast(false);

	return S_OK;
}

void CUI_GachaText::Update(_float dt)
{
}

CGameObject* CUI_GachaText::Create()
{
	CUI_GachaText* pInstance = new CUI_GachaText;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_GachaText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_GachaText::Clone(INIT_DESC* pArg)
{
	CUI_GachaText* pInstance = new CUI_GachaText(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_GachaText");
		Safe_Release(pInstance);
	}
	return pInstance;
}