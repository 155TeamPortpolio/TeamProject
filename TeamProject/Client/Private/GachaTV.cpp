#include "pch.h"
#include "GachaTV.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "StaticModel.h"
#include "Material.h"

#include "GachaScreen.h"

CGachaTV::CGachaTV()
    :CGameObject()
{
}

CGachaTV::CGachaTV(const CGachaTV& rhs)
    :CGameObject(rhs), m_pResultDesc(rhs.m_pResultDesc)
{
}

void CGachaTV::PlayTVSequence()
{
	m_pScreen->PlayTVSequence(m_pResultDesc);
}

void CGachaTV::SetupInitialTVSequence()
{
	m_pScreen->SetupInitialTVSequence(m_pResultDesc);
}

HRESULT CGachaTV::Initialize_Prototype(vector<WEAPON_DESC>* Desc)
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

	auto pModel = Add_Component<CStaticModel>();
	auto pMaterial = Add_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "TVNoScreen.model");
	pMaterial->Link_Material("Gacha_Level", "TVBlender1.mat");

	m_pResultDesc = Desc;

    return S_OK;
}

HRESULT CGachaTV::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_TVScreen();
    return S_OK;
}

void CGachaTV::Awake()
{
}

void CGachaTV::Priority_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Priority_UpdateChild(dt);
}

void CGachaTV::Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->UpdateChild(dt);
}

void CGachaTV::Late_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Late_UpdateChild(dt);
}

void CGachaTV::Add_TVScreen()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaScreen", CGachaScreen::Create());

	auto pObjectContainer = Add_Component<CObjectContainer>();
	COLLIDER_DESC colliderDesc{};
	colliderDesc.eType = COLLIDER_TYPE::BOX;
	colliderDesc.eGroup = COLLISION_GROUP::COMMON;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
	colliderDesc.bAutoFit = true;
	colliderDesc.bTrigger = true;

	CGameObject* gachaScreen = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaScreen" })
		.Collider(colliderDesc)
		.Build("Screen");

	pObjectContainer->Add_Child(gachaScreen, true);

	m_pScreen = dynamic_cast<CGachaScreen*>(gachaScreen);
}
 
CGachaTV* CGachaTV::Create(vector<WEAPON_DESC>* Desc)
{
	CGachaTV* Instance = new CGachaTV();
	if (FAILED(Instance->Initialize_Prototype(Desc)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaTV::Clone(INIT_DESC* pArg)
{
	CGachaTV* Instance = new CGachaTV(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaTV::Free()
{
	__super::Free();
}
