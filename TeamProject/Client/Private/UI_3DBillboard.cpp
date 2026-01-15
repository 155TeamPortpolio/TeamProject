#include "pch.h"
#include "UI_3DBillboard.h"

#include "StaticModel.h"
#include "Material.h"

CUI_3DBillboard::CUI_3DBillboard()
	:CGameObject()
{
}

CUI_3DBillboard::CUI_3DBillboard(const CUI_3DBillboard& rhs)
	:CGameObject(rhs)
{
}

HRESULT CUI_3DBillboard::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_3DBillboard::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Add_Component<CStaticModel>();
	auto pMaterial = Add_Component<CMaterial>();

	pModel->Link_Model("Test_Level", "UI_3DBillboard_Zero.model");
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
	pModel->ShadowCast(false);

	pMaterial->Link_Material("Test_Level", "UI_3DBillboard_Zero.mat");

	return S_OK;
}

void CUI_3DBillboard::Priority_Update(_float dt)
{
}

void CUI_3DBillboard::Update(_float dt)
{
}

void CUI_3DBillboard::Late_Update(_float dt)
{
}

CGameObject* CUI_3DBillboard::Create()
{
	CUI_3DBillboard* pInstance = new CUI_3DBillboard;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_3DBillboard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_3DBillboard::Clone(INIT_DESC* pArg)
{
	CUI_3DBillboard* pInstance = new CUI_3DBillboard;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_3DBillboard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_3DBillboard::Free()
{
	__super::Free();
}