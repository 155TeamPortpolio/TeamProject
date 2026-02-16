#include "pch.h"
#include "DogBone.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Material.h"
#include "AudioSource.h"

CDogBone::CDogBone()
    : CAmbientActor()
{
}

CDogBone::CDogBone(const CDogBone& rhs)
    : CAmbientActor(rhs)
{
}

HRESULT CDogBone::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    auto pResource = ResourceManager();

    //pResource->Add_ResourcePath("MainCity_Prop_NewsstandStuff_09out.model", "../Bin/Resources/MainCity/AmbientActor/DogBone/MainCity_Prop_NewsstandStuff_09out.model");
    //pResource->Add_ResourcePath("MainCity_Prop_NewsstandStuff_09out.mat", "../Bin/Resources/MainCity/AmbientActor/DogBone/MainCity_Prop_NewsstandStuff_09out.mat");
    //pResource->Add_ResourcePath("MainCity_Prop_NewsstandStuff_09out_Meta.json", "../Bin/Resources/MainCity/MainCity_Prop_NewsstandStuff_09out_Meta.json");

    auto pModel = Get_Component<CSkeletalModel>();
    pModel->Link_Model("MainCity_Level", "MainCity_Prop_NewsstandStuff_09out.model");
    auto pMaterial = Get_Component<CMaterial>();
    pMaterial->Link_Material("MainCity_Level", "MainCity_Prop_NewsstandStuff_09out.mat");

    return S_OK;
}

HRESULT CDogBone::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    AMBIENTACTOR_DESC* pDesc = static_cast<AMBIENTACTOR_DESC*>(pArg);

    m_AnimName = pDesc->strAnimName;
    m_bLoop = pDesc->bLoop;

    return S_OK;
}

void CDogBone::Awake()
{
    auto pAnimator = Get_Component<CAnimator3D>();
    pAnimator->LinkAnimate_Model("MainCity_Level", "MainCity_Prop_NewsstandStuff_09out.model");
    pAnimator->Link_MetaData("MainCity_Level", "MainCity_Prop_NewsstandStuff_09_Idle_Meta.json");

    pAnimator->Set_Animation(m_AnimName)
        .Loop(true)
        .Apply();
}

void CDogBone::Priority_Update(_float dt)
{
}

void CDogBone::Update(_float dt)
{
    Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CDogBone::Late_Update(_float dt)
{
}

CDogBone* CDogBone::Create()
{
    CDogBone* pInstance = new CDogBone();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to create : CDogBone");
    }

    return pInstance;
}

CGameObject* CDogBone::Clone(INIT_DESC* pArg)
{
    CDogBone* pInstance = new CDogBone(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to clone : CDogBone");
    }

    return pInstance;
}

void CDogBone::Free()
{
    __super::Free();
}
