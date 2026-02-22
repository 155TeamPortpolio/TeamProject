#include "pch.h"
#include "MilitaryHelicopter.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Material.h"
#include "AudioSource.h"

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
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CAnimator3D>();
    Add_Component<CSkeletalModel>();
    Add_Component<CMaterial>();
    Add_Component<CAudioSource>();

    auto pResource = ResourceManager();

    Get_Component<CAudioSource>()->SoundFolder("Scott_Level", "../Bin/Resources/Scott/AmbientActor/MilitaryHelicopter/Sound/");

    return S_OK;
}

HRESULT CMilitaryHelicopter::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    AMBIENTACTOR_DESC* pDesc = static_cast<AMBIENTACTOR_DESC*>(pArg);

    auto pModel = Get_Component<CSkeletalModel>();
    pModel->Link_Model("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.model");
    auto pMaterial = Get_Component<CMaterial>();
    pMaterial->Link_Material("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.mat");

    m_AnimName = pDesc->strAnimName;
    m_fDelayTime = pDesc->fDelay;

    return S_OK;
}

void CMilitaryHelicopter::Awake()
{
    auto pAnimator = Get_Component<CAnimator3D>();
    pAnimator->LinkAnimate_Model("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01out.model");
    pAnimator->Link_MetaData("Scott_Level", "Device_Vehicle_MilitaryHelicopter_01_Meta.json");
    m_bActive = false;
}

void CMilitaryHelicopter::Priority_Update(_float dt)
{
    if (m_bActive)  return;

    m_fDelayElapsed += dt;

    if (m_fDelayElapsed >= m_fDelayTime) {
        m_bActive = true;

        Get_Component<CAnimator3D>()->Set_Animation(m_AnimName)
            .Loop(true)
            .Apply();

        Get_Component<CAudioSource>()->Slot("Helicopter.wav")
            .Attribute3D(true)
            .Group(SOUND_GROUP::ENV)
            .Loop(true)
            .Volume(0.3f)
            .Play();
    }
}

void CMilitaryHelicopter::Update(_float dt)
{
    if (!m_bActive) return;

    Get_Component<CAnimator3D>()->Update_Animation(dt);
    Get_Component<CAudioSource>()->Set_AudioPos(
        Get_Component<CAnimator3D>()->Get_BonePosition(CAnimator3D::BoneSpace::WORLD, "Ctr_Main"));
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
