#include "pch.h"
#include "TestFloor.h"

#include "StaticModel.h"
#include "Material.h"
#include "Collider.h"
#include "ObjectContainer.h"

#include "GameInstance.h"

CTestFloor::CTestFloor()
{
}

CTestFloor::CTestFloor(const CTestFloor& rhs)
	:CGameObject(rhs)
{
}

HRESULT CTestFloor::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Concert_Ground_FloorTile_01.model",
		"../../DemoResource/testfloor/Concert_Ground_FloorTile_01.model");
	pRcsMgr->Add_ResourcePath("Concert_Ground_FloorTile_01.mat",
		"../../DemoResource/testfloor/Concert_Ground_FloorTile_01.mat");
	Get_Component<CStaticModel>()->Link_Model("Test_Level", "Concert_Ground_FloorTile_01.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Concert_Ground_FloorTile_01.mat");

	return S_OK;
}

HRESULT CTestFloor::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CTestFloor::Awake()
{
}

void CTestFloor::Priority_Update(_float dt)
{
}

void CTestFloor::Update(_float dt)
{
}

void CTestFloor::Late_Update(_float dt)
{
}

void CTestFloor::OnCollisionEnter()
{
}

void CTestFloor::OnCollisionStay()
{
}

void CTestFloor::OnCollisionExit()
{
}

void CTestFloor::Render_GUI()
{
	__super::Render_GUI();
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

CTestFloor* CTestFloor::Create()
{
	CTestFloor* instance = new CTestFloor();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTestFloor");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CTestFloor::Clone(INIT_DESC* pArg)
{
	CTestFloor* instance = new CTestFloor(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTestFloor");
		Safe_Release(instance);
	}

	return instance;
}

void CTestFloor::Free()
{
	__super::Free();
}
