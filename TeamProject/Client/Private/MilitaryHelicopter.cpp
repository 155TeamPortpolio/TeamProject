#include "pch.h"
#include "MilitaryHelicopter.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Material.h"

CMilitaryHelicopter::CMilitaryHelicopter()
    : CAmbientActor()
{
}

CMilitaryHelicopter::CMilitaryHelicopter(const CMilitaryHelicopter& rhs)
    : CAmbientActor(rhs)
{
}

HRESULT CMilitaryHelicopter::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    auto pResource = ResourceManager();

    pResource->Add_ResourcePath("Device_Vehicle_MilitaryHelicopter_01out.model", "../Bin/Resources/Scott/Map/AnimationModel/MilitaryHelicopter/Device_Vehicle_MilitaryHelicopter_01out.model");
    pResource->Add_ResourcePath("Device_Vehicle_MilitaryHelicopter_01out.mat", "../Bin/Resources/Scott/Map/AnimationModel/MilitaryHelicopter/Device_Vehicle_MilitaryHelicopter_01out.mat");
    pResource->Add_ResourcePath("Device_Vehicle_MilitaryHelicopter_01out_Meta.json", "../Bin/Resources/Scott/Device_Vehicle_MilitaryHelicopter_01_Meta.json");

    auto pModel = Get_Component<CSkeletalModel>();
    pModel->Link_Model("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.model");
    auto pMaterial = Get_Component<CMaterial>();
    pMaterial->Link_Material("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.mat");


    return S_OK;
}

HRESULT CMilitaryHelicopter::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    AMBIENTACTOR_DESC* pDesc = static_cast<AMBIENTACTOR_DESC*>(pArg);

    m_AnimName = pDesc->strAnimName;
    m_bLoop = pDesc->bLoop;

    return S_OK;
}

void CMilitaryHelicopter::Awake()
{
    auto pAnimator = Get_Component<CAnimator3D>();
    pAnimator->LinkAnimate_Model("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.model");
    pAnimator->Link_MetaData("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01_Meta.json");

    pAnimator->Set_Animation(m_AnimName)
        .Loop(true)
        .Apply();
}

void CMilitaryHelicopter::Priority_Update(_float dt)
{
}

void CMilitaryHelicopter::Update(_float dt)
{
    Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CMilitaryHelicopter::Late_Update(_float dt)
{
}

CMilitaryHelicopter* CMilitaryHelicopter::Create()
{
    CMilitaryHelicopter* pInstance = new CMilitaryHelicopter();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to create : CMilitaryHelicopter");
    }

    return pInstance;
}

CGameObject* CMilitaryHelicopter::Clone(INIT_DESC* pArg)
{
    CMilitaryHelicopter* pInstance = new CMilitaryHelicopter(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to clone : CMilitaryHelicopter");
    }

    return pInstance;
}

void CMilitaryHelicopter::Free()
{
    __super::Free();
}
