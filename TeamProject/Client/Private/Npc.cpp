#include "pch.h"
#include "Npc.h"

#include "UI_NameIndicator.h"
#include "NpcInteractZone.h"
#include "FieldSystem.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

CNpc::CNpc()
{
}

CNpc::CNpc(const CNpc& rhs)
    :CGameObject(rhs)
{
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

HRESULT CNpc::Add_InteractZone(_float4 vCenter, _float3 vSize)
{
    Add_Component<CObjectContainer>();
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();

    COLLIDER_DESC colliderDesc{};
    colliderDesc.eType = COLLIDER_TYPE::BOX;
    colliderDesc.eGroup = COLLISION_GROUP::INTERACABLE;
    colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
    colliderDesc.bAutoFit = false;
    colliderDesc.vCenter = _float3(0.f,0.f,1.5f);
    colliderDesc.vSize = vSize;
    colliderDesc.bTrigger = true;

    CGameObject* pInteractZone = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_NpcCollider" })
        .Position(_float3(vCenter.x, vCenter.y,vCenter.z))
        .Collider(colliderDesc)
        .Build("NpcCollider");

    pObjectContainer->Add_Child(pInteractZone, true);

    return S_OK;
}

void CNpc::Free()
{
    __super::Free();
}
