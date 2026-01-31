#include "pch.h"
#include "GachaProps.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"
#include "ObjectContainer.h"

#include "GachaBack.h"
#include "GachaTV.h"
#include "GachaStage.h"

CGachaProps::CGachaProps()
    :CGameObject()
{
}

CGachaProps::CGachaProps(const CGachaProps& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaProps::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CStaticModel>();
    Add_Component<CMaterial>();
    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CGachaProps::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Initialize_GlobalPrototype())) 
        return E_FAIL;

    Add_GachaProps();
    return S_OK;
}

void CGachaProps::Awake()
{
}

void CGachaProps::Priority_Update(_float dt)
{
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
    pObjectContainer->Priority_UpdateChild(dt);
}

void CGachaProps::Update(_float dt)
{
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
    pObjectContainer->UpdateChild(dt);
}

void CGachaProps::Late_Update(_float dt)
{
    CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
    pObjectContainer->Late_UpdateChild(dt);
}

HRESULT CGachaProps::Initialize_GlobalPrototype()
{
    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaBack", CGachaBack::Create());
    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaTV", CGachaTV::Create());
    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaStage", CGachaStage::Create());
    return S_OK;
}

void CGachaProps::Add_GachaProps()
{
    auto pObjectContainer = Get_Component<CObjectContainer>();
    COLLIDER_DESC colliderDesc{};
    colliderDesc.eType = COLLIDER_TYPE::BOX;
    colliderDesc.eGroup = COLLISION_GROUP::COMMON;
    colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
    colliderDesc.bAutoFit = true;
    colliderDesc.bTrigger = true;

    CGameObject* gachaBack = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_GachaBack" })
        .Collider(colliderDesc)
        .Build("GachaLayer");

    pObjectContainer->Add_Child(gachaBack, true);

    CGameObject* gachaTV = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_GachaTV" })
        .Collider(colliderDesc)
        .Build("GachaLayer");

    pObjectContainer->Add_Child(gachaTV, true);

    CGameObject* gachaStage = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_GachaStage" })
        .Collider(colliderDesc)
        .Build("GachaLayer");

    pObjectContainer->Add_Child(gachaStage, true);
}

CGachaProps* CGachaProps::Create()
{
	CGachaProps* Instance = new CGachaProps();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaProps::Clone(INIT_DESC* pArg)
{
	CGachaProps* Instance = new CGachaProps(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaProps::Free()
{
	__super::Free();
}
