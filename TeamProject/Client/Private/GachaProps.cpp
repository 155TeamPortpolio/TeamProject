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
#include "GachaScreen.h"

CGachaProps::CGachaProps()
    :CGameObject()
{
}

CGachaProps::CGachaProps(const CGachaProps& rhs)
    :CGameObject(rhs), m_pResultDesc(rhs.m_pResultDesc)
{
}

void CGachaProps::SetupInitialSequence()
{
    m_pTV->SetupInitialTVSequence();
}

void CGachaProps::PlayTVSequence()
{
    m_pTV->PlayTVSequence();
}

void CGachaProps::PlayStageSpin(_int index)
{
    m_pStage->PlayStageSpin(index);
}

HRESULT CGachaProps::Initialize_Prototype(vector<GACHA_RESULT_DESC>* Desc)
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();
    m_pResultDesc = Desc;

    return S_OK;
}

HRESULT CGachaProps::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Initialize_GachaPrototype())) 
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

HRESULT CGachaProps::Initialize_GachaPrototype()
{
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaBack", CGachaBack::Create());
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaTV", CGachaTV::Create(m_pResultDesc));
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaStage", CGachaStage::Create(m_pResultDesc));
    return S_OK;
}

void CGachaProps::Add_GachaProps()
{
    auto pObjectContainer = Get_Component<CObjectContainer>();
    COLLIDER_DESC colliderDesc{};
    colliderDesc.eType = COLLIDER_TYPE::BOX;
    colliderDesc.eGroup = COLLISION_GROUP::COMMON;
    colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
    colliderDesc.bAutoFit = false;
    colliderDesc.vSize = _float3(3.754, 0.460, 3.524);
    colliderDesc.vCenter = _float3(0.040, -0.083, -0.916);

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

    m_pStage = dynamic_cast<CGachaStage*>(gachaStage);
    m_pTV = dynamic_cast<CGachaTV*>(gachaTV);
}

CGachaProps* CGachaProps::Create(vector<GACHA_RESULT_DESC>* Desc)
{
	CGachaProps* Instance = new CGachaProps();
	if (FAILED(Instance->Initialize_Prototype(Desc)))
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
