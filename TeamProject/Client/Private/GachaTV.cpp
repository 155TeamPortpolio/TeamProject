#include "pch.h"
#include "GachaTV.h"

#include "StaticModel.h"
#include "Material.h"
#include "Child.h"

CGachaTV::CGachaTV()
    :CGameObject()
{
}

CGachaTV::CGachaTV(const CGachaTV& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaTV::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

	auto pModel = Add_Component<CStaticModel>();
	auto pMaterial = Add_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "GachaStage_Televisonout.model");
	pMaterial->Link_Material("Gacha_Level", "GachaStage_Televisonout.mat");

	pModel->SetDrawable(33, false);

    return S_OK;
}

HRESULT CGachaTV::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    return S_OK;
}

void CGachaTV::Awake()
{
}

void CGachaTV::Priority_Update(_float dt)
{
}

void CGachaTV::Update(_float dt)
{
}

void CGachaTV::Late_Update(_float dt)
{
}

CGachaTV* CGachaTV::Create()
{
	CGachaTV* Instance = new CGachaTV();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaTV::Clone(INIT_DESC* pArg)
{
	CGachaTV* Instance = new CGachaTV(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaTV::Free()
{
	__super::Free();
}
