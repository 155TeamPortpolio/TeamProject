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
    :CGachaResult(rhs)
{
}

void CGachaAvatar::SetResult(string strModel, string strMaterial, _float4 vRot)
{
    auto pModel = Get_Component<CSkeletalModel>();
    auto pMaterial = Get_Component<CMaterial>();
    
    pModel->Link_Model(G_GlobalLevelKey, "Miyabi.model");
    pMaterial->Link_Material(G_GlobalLevelKey, "Miyabi.mat");

    m_pAnimator = Get_Component<CAnimator3D>();
    m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Miyabi.model");
    m_pAnimator->Link_MetaData(G_GlobalLevelKey, "Miyabi_Meta.json");

    m_pAnimator->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Gacha_Start")
        .Loop(false)
        .Apply();

    m_pTransform->Set_Pos(_float4(0.f, 0.2f, -1.6f, 0.f));
    m_pTransform->Rotate(_float3(0.f, XM_PI, 0.f));
}

HRESULT CGachaAvatar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CSkeletalModel>();
    auto pMaterial = Add_Component<CMaterial>();
    Add_Component<CAnimator3D>();

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
    m_pAnimator = Get_Component<CAnimator3D>();
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
    Update_States();
    __super::Update(dt);
}

void CGachaAvatar::Late_Update(_float dt)
{
    __super::Late_Update(dt);
}

void CGachaAvatar::Update_States()
{
    switch (m_eAnimState)
    {
    case ANIMSTATE::START:
        if (m_pAnimator->isCurrentAnimEnd())
        {
            m_pAnimator->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Gacha_Loop")
                .Loop(true)
                .Apply();
            m_eAnimState = ANIMSTATE::LOOP;
        }
        break;
    case ANIMSTATE::LOOP:
        break;
    }
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
