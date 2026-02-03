#include "pch.h"
#include "GachaAvatar.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

#include "Helper_Func.h"

CGachaAvatar::CGachaAvatar()
    :CGachaResult()
{
}

CGachaAvatar::CGachaAvatar(const CGachaAvatar& rhs)
    :CGachaResult(rhs), m_pAnimator(rhs.m_pAnimator)
{
}

void CGachaAvatar::SetResult(string strModel, string strMaterial, _float4 vRot)
{
    //auto pModel = Get_Component<CSkeletalModel>();
    //auto pMaterial = Get_Component<CMaterial>();
    //
    //pModel->Link_Model(G_GlobalLevelKey, "Miyabi.model");
    //pMaterial->Link_Material(G_GlobalLevelKey, "Miyabi.mat");
}

HRESULT CGachaAvatar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CSkeletalModel>();
    auto pMaterial = Add_Component<CMaterial>();
    m_pAnimator = Add_Component<CAnimator3D>();

    pModel->Link_Model(G_GlobalLevelKey, "Miyabi.model");
    pMaterial->Link_Material(G_GlobalLevelKey, "Miyabi.mat");

    return S_OK;
}

HRESULT CGachaAvatar::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaAvatar::Awake()
{
    __super::Awake();
    m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Miyabi.model");
    m_pAnimator->Link_MetaData(G_GlobalLevelKey, "Miyabi_Meta.json");

    m_pAnimator->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Gacha_Loop")
        .Loop(true)
        .Apply();
}

void CGachaAvatar::Priority_Update(_float dt)
{
    __super::Priority_Update(dt);
}

void CGachaAvatar::Update(_float dt)
{
    m_pAnimator->Update_Animation(dt);
    __super::Update(dt);
}

void CGachaAvatar::Late_Update(_float dt)
{
    __super::Late_Update(dt);
}

CGachaAvatar* CGachaAvatar::Create()
{
    CGachaAvatar* Instance = new CGachaAvatar();
    if (FAILED(Instance->Initialize_Prototype()))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

CGameObject* CGachaAvatar::Clone(INIT_DESC* pArg)
{
    CGachaAvatar* Instance = new CGachaAvatar(*this);
    if (FAILED(Instance->Initialize(pArg)))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

void CGachaAvatar::Free()
{
    __super::Free();
}
