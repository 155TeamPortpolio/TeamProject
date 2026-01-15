#include "pch.h"
#include "UI_MeshBillboard.h"

#include "StaticModel.h"
#include "Material.h"

CUI_MeshBillboard::CUI_MeshBillboard()
	:CGameObject()
{
}

CUI_MeshBillboard::CUI_MeshBillboard(const CUI_MeshBillboard& rhs)
	:CGameObject(rhs)
{
}

HRESULT CUI_MeshBillboard::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_MeshBillboard::Initialize(INIT_DESC* pArg)
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

void CUI_MeshBillboard::Priority_Update(_float dt)
{
}

void CUI_MeshBillboard::Update(_float dt)
{
}

void CUI_MeshBillboard::Late_Update(_float dt)
{
}

CGameObject* CUI_MeshBillboard::Create()
{
	CUI_MeshBillboard* pInstance = new CUI_MeshBillboard;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_MeshBillboard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MeshBillboard::Clone(INIT_DESC* pArg)
{
	CUI_MeshBillboard* pInstance = new CUI_MeshBillboard;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_MeshBillboard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MeshBillboard::Free()
{
	__super::Free();
}