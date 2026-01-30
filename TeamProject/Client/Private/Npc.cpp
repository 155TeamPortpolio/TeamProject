#include "pch.h"
#include "Npc.h"

#include "UI_NameIndicator.h"
#include "NpcInteractZone.h"
#include "FieldSystem.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "Material.h"
#include "MaterialInstance.h"
#include "Texture.h"

CNpc::CNpc()
{
}

CNpc::CNpc(const CNpc& rhs)
    :CGameObject(rhs)
{
}

void CNpc::OnCameraCollision(const CamOcclusionHit& hit)
{
    auto pMaterial = Get_Component<CMaterial>();
    auto& materialInstances = pMaterial->Get_MaterialInstances();

    if (hit.phase == CamOcclusionPhase::Enter)
    {
        for (const auto& instance : materialInstances)
            instance->Override_Pass("TransparentNoise");

        m_fCamFadeAlpha = 0.6f;
        return;
    }

    if (hit.phase == CamOcclusionPhase::Exit)
    {
        for (const auto& instance : materialInstances)
            instance->Reset_Pass();

        m_fCamFadeAlpha = 0.0f;
        return;
    }
}

HRESULT CNpc::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void CNpc::Awake()
{
    auto pMaterial = Get_Component<CMaterial>();
    auto& materialInstances = pMaterial->Get_MaterialInstances();
    auto DitheringTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Noise_032.png");

    if (DitheringTexture == nullptr) return;
    for (const auto& Instance : materialInstances)
    {
        Instance->Set_Param("DitheringTexture", { DitheringTexture->Get_SRV(),"Texture2D",0 });
        pMaterial->Add_MaterialData(Instance, "fCameraFadeAlpha", { &m_fCamFadeAlpha, "float", sizeof(_float) });
    }
}

void CNpc::Priority_Update(_float dt)
{
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
    if (pObjectContainer) pObjectContainer->Priority_UpdateChild(dt);
}

void CNpc::Update(_float dt)
{
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
    if (pObjectContainer) pObjectContainer->UpdateChild(dt);
}

void CNpc::Late_Update(_float dt)
{
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
    if (pObjectContainer) pObjectContainer->Late_UpdateChild(dt);
}

HRESULT CNpc::Add_NameIndicator()
{
    CUI_NameIndicator::INDICATOR_DESC* pDesc = new CUI_NameIndicator::INDICATOR_DESC;
    pDesc->strName = m_strName;
    pDesc->pCCT = Get_Component<CCharacterController>();
    auto pUI = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_NameIndicator" })
        .Add_UIDesc(pDesc)
        .Build("nameIndicator");
    UIManager()->Add_UIObject(pUI, LevelManager()->Get_NowLevelKey());

    return S_OK;
}

HRESULT CNpc::Add_InteractZonePrototype()
{
    if (FAILED(PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_NpcCollider", CNpcInteractZone::Create())))
        return E_FAIL;
    return S_OK;
}

HRESULT CNpc::Add_InteractZone(_float4 vCenter, _float3 vOffset, _float3 vRotate, _float3 vSize)
{
    Add_InteractZonePrototype();

    CObjectContainer* pObjectContainer = Add_Component<CObjectContainer>();

    RIGIDBODY_DESC rigidDesc{};
    rigidDesc.isKinematic = true;
    rigidDesc.bEnableGravity = false;

    COLLIDER_DESC colliderDesc{};
    colliderDesc.eType = COLLIDER_TYPE::BOX;
    colliderDesc.eGroup = COLLISION_GROUP::INTERACTABLE;
    colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
    colliderDesc.bAutoFit = false;
    colliderDesc.vCenter = vOffset;
    colliderDesc.vSize = vSize;
    colliderDesc.bTrigger = true;

    CGameObject* pInteractZone = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_NpcCollider" })
        .Position(_float3(vCenter.x, vCenter.y,vCenter.z))
        .Rotate(vRotate)
        .RigidBody(rigidDesc)
        .Collider(colliderDesc)
        .Build("NpcCollider");

    pObjectContainer->Add_Child(pInteractZone, false);

    return S_OK;
}

void CNpc::Free()
{
    __super::Free();
}
