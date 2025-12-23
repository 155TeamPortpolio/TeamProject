#pragma once
#include "pch.h"
#include "TestMap.h"
#include "Material.h"
#include "GameInstance.h"
#include "ObjectContainer.h"
#include "StaticModel.h"

HRESULT CTestMap::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	auto resMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	string defaultPath = "../bin/Resources/Binary_MapData/";
	string folderName  = "MainCity_Sub";
	string fileName    = "Meshout";

	resMgr->Add_ResourcePath(fileName + ".model", defaultPath + folderName + "/" + fileName + ".model");
	resMgr->Add_ResourcePath(fileName + ".mat",   defaultPath + folderName + "/" + fileName + ".mat"  );

	//Add_Component<CCollider>();
	return S_OK;
}

HRESULT CTestMap::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	Get_Component<CModel>()->Link_Model("Test_Level", "Meshout.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Meshout.mat");

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CTestMap::Awake()
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