#include "pch.h"
#include "GachaFootStage.h"

#include "SkeletalModel.h"
#include "Material.h"

CGachaFootStage::CGachaFootStage()
    :CGameObject()
{
}

CGachaFootStage::CGachaFootStage(const CGachaFootStage& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaFootStage::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CSkeletalModel>();
    auto pMaterial = Add_Component<CMaterial>();

    pModel->Link_Model("Gacha_Level", "GachaStage_Prop_AvatarPlatform_02out.model");
    pMaterial->Link_Material("Gacha_Level", "GachaStage_Prop_AvatarPlatform_02out.mat");

    Add_Component<CCollider>();

    return S_OK;
}

HRESULT CGachaFootStage::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaFootStage::Awake()
{
    __super::Awake();
}

void CGachaFootStage::Priority_Update(_float dt)
{
}

void CGachaFootStage::Update(_float dt)
{
}

void CGachaFootStage::Late_Update(_float dt)
{
}

CGachaFootStage* CGachaFootStage::Create()
{
    CGachaFootStage* Instance = new CGachaFootStage();
    if (FAILED(Instance->Initialize_Prototype()))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

CGameObject* CGachaFootStage::Clone(INIT_DESC* pArg)
{
    CGachaFootStage* Instance = new CGachaFootStage(*this);
    if (FAILED(Instance->Initialize(pArg)))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

void CGachaFootStage::Free()
{
    __super::Free();
}
