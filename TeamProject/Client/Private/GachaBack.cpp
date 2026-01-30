#include "pch.h"
#include "GachaBack.h"

#include "StaticModel.h"
#include "Material.h"
#include "Child.h"

CGachaBack::CGachaBack()
    :CGameObject()
{
}

CGachaBack::CGachaBack(const CGachaBack& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaBack::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CStaticModel>();
    Add_Component<CMaterial>();

    Get_Component<CStaticModel>()->Link_Model("Gacha_Level", "GachaStageout.model");
    Get_Component<CMaterial>()->Link_Material("Gacha_Level", "GachaStageout.mat");

	Get_Component<CModel>()->ShadowCast(false);
    return S_OK;
}

HRESULT CGachaBack::Initialize(INIT_DESC* pArg)
{
    return S_OK;
}

void CGachaBack::Awake()
{
}

void CGachaBack::Priority_Update(_float dt)
{
}

void CGachaBack::Update(_float dt)
{
}

void CGachaBack::Late_Update(_float dt)
{
}

CGachaBack* CGachaBack::Create()
{
	CGachaBack* Instance = new CGachaBack();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaBack::Clone(INIT_DESC* pArg)
{
	CGachaBack* Instance = new CGachaBack(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaBack::Free()
{
	__super::Free();
}
