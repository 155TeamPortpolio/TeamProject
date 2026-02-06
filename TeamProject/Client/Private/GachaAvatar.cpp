#include "pch.h"
#include "GachaAvatar.h"

#include "GachaStage.h"
#include "GachaFootStage.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "RigidBody.h"
#include "Child.h"
#include "ObjectContainer.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "CamDirector.h"

CGachaAvatar::CGachaAvatar()
    :CGachaResult()
{
}

CGachaAvatar::CGachaAvatar(const CGachaAvatar& rhs)
    :CGachaResult(rhs)
{
}

void CGachaAvatar::SetResult(GACHA_RESULT_DESC Desc)
{
    auto pModel = Get_Component<CSkeletalModel>();
    auto pMaterial = Get_Component<CMaterial>();
    
    pModel->Link_Model(G_GlobalLevelKey, Desc.strModel);
    pMaterial->Link_Material(G_GlobalLevelKey, Desc.strMaterial);

    pModel->Hide_MehsByName("PET");

    m_pAnimator = Get_Component<CAnimator3D>();
    m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, Desc.strModel);
    m_pAnimator->Link_MetaData(G_GlobalLevelKey, Desc.strMeta);

    m_pAnimator->Set_Pause(false);
    m_pAnimator->Set_Animation(Desc.strStartAnim)
        .Loop(false)
        .Apply();

    strLoopAnim = Desc.strLoopAnim;
    m_eAnimState = ANIMSTATE::START;

    m_pTransform->Rotate(_float3(0.f, XM_PI, 0.f));
    m_bRevealEffect = false;
}

void CGachaAvatar::SetRenderState(_bool Render)
{
    __super::SetRenderState(Render);
    if(Render == false) m_pAnimator->Set_Pause(true);
}

HRESULT CGachaAvatar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CSkeletalModel>();
    auto pMaterial = Add_Component<CMaterial>();
    Add_Component<CAnimator3D>();
    Add_Component<CRigidBody>();
    Add_Component<CCollider>();

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
    Get_Component<CRigidBody>()->Late_Update(dt);
    __super::Late_Update(dt);
}

void CGachaAvatar::Update_States()
{
    switch (m_eAnimState)
    {
    case ANIMSTATE::START:
        if (m_pAnimator->isCurrentAnimEnd())
        {
            m_pAnimator->Change_Animation(strLoopAnim)
                .Loop(true)
                .Apply();
            m_eAnimState = ANIMSTATE::LOOP;
        }
        else if(m_pAnimator->Get_CurAnimDuration() >= 0.35)
        {
            if (m_bRevealEffect == false) 
            {
                auto pParent = dynamic_cast<CGachaStage*>(Get_Component<CChild>()->Get_Parent());
                pParent->PlayRevealEffect();
                m_bRevealEffect = true;

                CamDirector()->SetSpaceRef(Get_Handle());
            }
        }
        break;
    case ANIMSTATE::LOOP:
        if (m_bSequencePlay == false)
        {
            CamDirector()->RequestSequence("Gacha/Miyabi_01");
            m_bSequencePlay = true;
        }
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
