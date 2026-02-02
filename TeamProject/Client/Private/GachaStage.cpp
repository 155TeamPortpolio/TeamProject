#include "pch.h"
#include "GachaStage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "StaticModel.h"
#include "Material.h"

#include "GachaStageScreen.h"

CGachaStage::CGachaStage()
    :CGameObject()
{
}

CGachaStage::CGachaStage(const CGachaStage& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaStage::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "AvatarScreen1out.model");
	pMaterial->Link_Material("Gacha_Level", "AvatarScreen1out.mat");

    return S_OK;
}

HRESULT CGachaStage::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_StageScreen();
    return S_OK; 
}

void CGachaStage::Awake()
{
	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "BangBooNoScreen1.model");
	pMaterial->Link_Material("Gacha_Level", "BangBooNoScreen1.mat");

	pModel->Hide_MehsByName("0023_GachaStage_Prop_TV_04_mesh0023");
}

void CGachaStage::Priority_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Priority_UpdateChild(dt);
}

void CGachaStage::Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->UpdateChild(dt);
}

void CGachaStage::Late_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Late_UpdateChild(dt);
}

void CGachaStage::Add_StageScreen()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaStageScreen", CGachaStageScreen::Create());

	auto pObjectContainer = Add_Component<CObjectContainer>();
	COLLIDER_DESC colliderDesc{};
	colliderDesc.eType = COLLIDER_TYPE::BOX;
	colliderDesc.eGroup = COLLISION_GROUP::COMMON;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
	colliderDesc.bAutoFit = true;
	colliderDesc.bTrigger = true;

	CGameObject* gachaStageScreen = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaStageScreen" })
		.Collider(colliderDesc)
		.Build("Screen");

	pObjectContainer->Add_Child(gachaStageScreen, true);
}

CGachaStage* CGachaStage::Create()
{
	CGachaStage* Instance = new CGachaStage();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaStage::Clone(INIT_DESC* pArg)
{
	CGachaStage* Instance = new CGachaStage(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaStage::Free()
{
	__super::Free();
}
