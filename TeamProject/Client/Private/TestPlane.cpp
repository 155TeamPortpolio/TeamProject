#include "pch.h"
#include "TestPlane.h"
#include "Material.h"
#include "GameInstance.h"
#include "ObjectContainer.h"
#include "StaticModel.h"

HRESULT CTestPlane::Initialize_Prototype()
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

HRESULT CTestPlane::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	return S_OK;
}

CTestPlane* CTestPlane::Create()
{
	CTestPlane* instance = new CTestPlane();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTestPlane");
		Safe_Release(instance);
	}
	return instance;
}

CGameObject* CTestPlane::Clone(INIT_DESC* pArg)
{
	CTestPlane* instance = new CTestPlane(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTestPlane");
		Safe_Release(instance);
	}
	return instance;
}