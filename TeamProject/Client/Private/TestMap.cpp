#pragma once
#include "pch.h"
#include "TestMap.h"
#include "Material.h"
#include "GameInstance.h"
#include "ObjectContainer.h"
#include "StaticModel.h"

CTestMap::CTestMap()
{
}

CTestMap::CTestMap(const CTestMap& rhs)
	:CGameObject(rhs)
{
}

HRESULT CTestMap::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Concert_Ground_FloorTile_01.model",
		"../../DemoResource/testfloor/Concert_Ground_FloorTile_01.model");
	pRcsMgr->Add_ResourcePath("Concert_Ground_FloorTile_01.mat",
		"../../DemoResource/testfloor/Concert_Ground_FloorTile_01.mat");
	Get_Component<CModel>()->Link_Model("Test_Level", "Concert_Ground_FloorTile_01.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Concert_Ground_FloorTile_01.mat");

	Add_Component<CCollider>();
	return S_OK;
}

HRESULT CTestMap::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CTestMap::Awake()
{

}

void CTestMap::Priority_Update(_float dt)
{
}

void CTestMap::Update(_float dt)
{

}

void CTestMap::Late_Update(_float dt)
{
}

void CTestMap::OnCollisionEnter()
{
}

void CTestMap::OnCollisionStay()
{
}

void CTestMap::OnCollisionExit()
{
}

void CTestMap::Render_GUI()
{
	__super::Render_GUI();
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

CTestMap* CTestMap::Create()
{
	CTestMap* instance = new CTestMap();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTestMap");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CTestMap::Clone(INIT_DESC* pArg)
{
	CTestMap* instance = new CTestMap(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTestMap");
		Safe_Release(instance);
	}

	return instance;
}

void CTestMap::Free()
{
	__super::Free();
}
