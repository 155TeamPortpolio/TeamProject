#include "pch.h"
#include "GachaResult.h"

#include "StaticModel.h"
#include "Material.h"

CGachaResult::CGachaResult()
    :CGameObject()
{
}

CGachaResult::CGachaResult(const CGachaResult& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaResult::LinkModel(const string& strModelName)
{
    auto pModel = Get_Component<CStaticModel>();
    pModel->Link_Model("Gacha_Level", strModelName);

    return S_OK;
}

HRESULT CGachaResult::LinkMaterial(const string& strMaterialName)
{
    auto pMaterial = Get_Component<CMaterial>();
    pMaterial->Link_Material("Gacha_Level", strMaterialName);

    return S_OK;
}

HRESULT CGachaResult::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

    pModel->Link_Model("Gacha_Level", "Weapon_A_Common_03out.model");
    pMaterial->Link_Material("Gacha_Level", "Weapon_A_Common_03out.mat");

    return S_OK;
}

HRESULT CGachaResult::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaResult::Awake()
{
}

void CGachaResult::Priority_Update(_float dt)
{
}

void CGachaResult::Update(_float dt)
{
}

void CGachaResult::Late_Update(_float dt)
{
}

CGachaResult* CGachaResult::Create()
{
    CGachaResult* Instance = new CGachaResult();
    if (FAILED(Instance->Initialize_Prototype()))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

CGameObject* CGachaResult::Clone(INIT_DESC* pArg)
{
    CGachaResult* Instance = new CGachaResult(*this);
    if (FAILED(Instance->Initialize(pArg)))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

void CGachaResult::Free()
{
    __super::Free();
}
