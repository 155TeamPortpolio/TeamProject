#include "pch.h"
#include "GachaStage.h"

#include "StaticModel.h"
#include "Material.h"
#include "Child.h"

CGachaStage::CGachaStage()
    :CGameObject()
{
}

CGachaStage::CGachaStage(const CGachaStage& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaStage::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "GachaStage_Avatarout.model");
	pMaterial->Link_Material("Gacha_Level", "GachaStage_Avatarout.mat");

    return S_OK;
}

HRESULT CGachaStage::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    return S_OK; 
}

void CGachaStage::Awake()
{
	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "GachaStage_Bangbooout.model");
	pMaterial->Link_Material("Gacha_Level", "GachaStage_Bangbooout.mat");
}

void CGachaStage::Priority_Update(_float dt)
{
}

void CGachaStage::Update(_float dt)
{
}

void CGachaStage::Late_Update(_float dt)
{
}

CGachaStage* CGachaStage::Create()
{
	CGachaStage* Instance = new CGachaStage();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaStage::Clone(INIT_DESC* pArg)
{
	CGachaStage* Instance = new CGachaStage(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaStage::Free()
{
	__super::Free();
}
