#include "pch.h"
#include "GachaProps.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"
#include "ObjectContainer.h"

#include "GachaBack.h"
#include "GachaTV.h"
#include "GachaStage.h"
#include "GachaResult.h"

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
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaBack", CGachaBack::Create());
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaTV", CGachaTV::Create());
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaStage", CGachaStage::Create());
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaResult", CGachaResult::Create());
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

    CGameObject* gachaBack = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaBack" })
        .Collider(colliderDesc)
        .Build("Back");

    pObjectContainer->Add_Child(gachaBack, true);

    CGameObject* gachaTV = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaTV" })
        .Collider(colliderDesc)
        .Build("TV");

    pObjectContainer->Add_Child(gachaTV, true);

    CGameObject* gachaStage = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaStage" })
        .Collider(colliderDesc)
        .Build("Stage");

    pObjectContainer->Add_Child(gachaStage, true);

    CGameObject* gachaResult = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaResult" })
        .Collider(colliderDesc)
        .Position(_float3(0.f, 1.45f, -1.5f))
        .Scale(_float3(2.f,2.f,2.f))
        .Build("Result");
    gachaResult->Get_Component<CTransform>()->Set_Quaternion(_vector4(-0.10, 0.80, -0.28, 0.52));
    
    pObjectContainer->Add_Child(gachaResult, true);
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
