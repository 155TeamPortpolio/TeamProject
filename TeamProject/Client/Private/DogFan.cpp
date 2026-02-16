#include "pch.h"
#include "DogFan.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Material.h"
#include "AudioSource.h"

CDogFan::CDogFan()
    : CAmbientActor()
{
}

CDogFan::CDogFan(const CDogFan& rhs)
    : CAmbientActor(rhs)
{
}

HRESULT CDogFan::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    auto pResource = ResourceManager();

    pResource->Add_ResourcePath("MainCity_Prop_NewsstandStuff_09out.model", "../Bin/Resources/MainCity/AmbientActor/DogBone/MainCity_Prop_NewsstandStuff_09out.model");
    pResource->Add_ResourcePath("MainCity_Prop_NewsstandStuff_09out.mat", "../Bin/Resources/Scott/AmbientActor/DogBone/MainCity_Prop_NewsstandStuff_09out.mat");
    pResource->Add_ResourcePath("MainCity_Prop_NewsstandStuff_09out_Meta.json", "../Bin/Resources/Scott/MainCity_Prop_NewsstandStuff_09out_Meta.json");

    auto pModel = Get_Component<CSkeletalModel>();
    pModel->Link_Model("MainCity_Level", "MainCity_Prop_NewsstandStuff_09out.model");
    auto pMaterial = Get_Component<CMaterial>();
    pMaterial->Link_Material("MainCity_Level", "MainCity_Prop_NewsstandStuff_09out.mat");

    return S_OK;
}

HRESULT CDogFan::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    AMBIENTACTOR_DESC* pDesc = static_cast<AMBIENTACTOR_DESC*>(pArg);

    m_AnimName = pDesc->strAnimName;
    m_bLoop = pDesc->bLoop;

    return S_OK;
}

void CDogFan::Awake()
{
    auto pAnimator = Get_Component<CAnimator3D>();
    pAnimator->LinkAnimate_Model("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.model");
    pAnimator->Link_MetaData("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01_Meta.json");

    pAnimator->Set_Animation(m_AnimName)
        .Loop(true)
        .Apply();

    Get_Component<CAudioSource>()->Slot("Helicopter.wav")
        .Attribute3D(true)
        .Group(SOUND_GROUP::ENV)
        .Loop(true)
        .Volume(0.15f)
        .Play();
}

void CDogFan::Priority_Update(_float dt)
{
}

void CDogFan::Update(_float dt)
{
    Get_Component<CAnimator3D>()->Update_Animation(dt);
    Get_Component<CAudioSource>()->Set_AudioPos(
        Get_Component<CAnimator3D>()->Get_BonePosition(CAnimator3D::BoneSpace::WORLD, "Ctr_Main"));
}

void CDogFan::Late_Update(_float dt)
{
}

CDogFan* CDogFan::Create()
{
    CDogFan* pInstance = new CDogFan();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to create : CDogFan");
    }

    return pInstance;
}

CGameObject* CDogFan::Clone(INIT_DESC* pArg)
{
    CDogFan* pInstance = new CDogFan(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to clone : CDogFan");
    }

    return pInstance;
}

void CDogFan::Free()
{
    __super::Free();
}
