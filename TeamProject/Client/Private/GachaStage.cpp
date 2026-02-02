#include "pch.h"
#include "GachaStage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "StaticModel.h"
#include "Material.h"

#include "GachaStageScreen.h"
#include "GachaResult.h"

CGachaStage::CGachaStage()
    :CGameObject()
{
}

CGachaStage::CGachaStage(const CGachaStage& rhs)
    :CGameObject(rhs), m_pResultDesc(rhs.m_pResultDesc), m_pIndex(rhs.m_pIndex)
{
}

HRESULT CGachaStage::Initialize_Prototype(vector<WEAPON_DESC>* Desc, _int* Index)
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "AvatarScreen1out.model");
	pMaterial->Link_Material("Gacha_Level", "AvatarScreen1out.mat");

	m_pResultDesc = Desc;
	m_pIndex = Index;

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

	Update_StageEnviroment();
}

void CGachaStage::Late_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Late_UpdateChild(dt);
}

void CGachaStage::Add_StageScreen()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaStageScreen", CGachaStageScreen::Create());
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaResult", CGachaResult::Create());

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

	m_pScreen = dynamic_cast<CGachaStageScreen*>(gachaStageScreen);

	pObjectContainer->Add_Child(gachaStageScreen, true);

	CGameObject* gachaResult = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaResult" })
		.Collider(colliderDesc)
		.Position(_float3(0.f, 1.45f, -1.5f))
		.Scale(_float3(2.f, 2.f, 2.f))
		.Build("Result");
	gachaResult->Get_Component<CTransform>()->Set_Quaternion(_vector4(-0.10, 0.80, -0.28, 0.52));

	m_pResult = dynamic_cast<CGachaResult*>(gachaResult);

	pObjectContainer->Add_Child(gachaResult, true);
}

void CGachaStage::Set_Stage(GACHA_STAGE eStage)
{
	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	if (eStage == GACHA_STAGE::AVATAR)
	{
		pModel->Link_Model("Gacha_Level", "AvatarScreen1out.model");
		pMaterial->Link_Material("Gacha_Level", "AvatarScreen1out.mat");
	}
	else
	{
		pModel->Link_Model("Gacha_Level", "BangBooNoScreen1.model");
		pMaterial->Link_Material("Gacha_Level", "BangBooNoScreen1.mat");

		pModel->Hide_MehsByName("0023_GachaStage_Prop_TV_04_mesh0023");
	}
}

void CGachaStage::Update_StageEnviroment()
{
	if (m_iPreIndex == *m_pIndex) return;
	m_iPreIndex = *m_pIndex;

	WEAPON_DESC CurrentDesc = (*m_pResultDesc)[m_iPreIndex];
	if (CurrentDesc.Grade == GachaGrade::S)
	{
		//m_pScreen->SetScreen(GACHA_STAGE::AVATAR, CurrentDesc.Grade);
		//Set_Stage(GACHA_STAGE::AVATAR);
	}
	else
	{
		//m_pScreen->SetScreen(GACHA_STAGE::BANGBOO, CurrentDesc.Grade);
		//Set_Stage(GACHA_STAGE::BANGBOO);
		//m_pResult->SetResult(CurrentDesc.strModel, CurrentDesc.strMaterial);
	}
}

CGachaStage* CGachaStage::Create(vector<WEAPON_DESC>* Desc, _int* Index)
{
	CGachaStage* Instance = new CGachaStage();
	if (FAILED(Instance->Initialize_Prototype(Desc, Index)))
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
