#include "pch.h"
#include "DemoFloor.h"
#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"
#include "ObjectContainer.h"


CDemoFloor::CDemoFloor()
{
}

CDemoFloor::CDemoFloor(const CDemoFloor& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDemoFloor::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Zero_Vehicle_Bus_01.model",
		"../../DemoResource/static/Zero_Vehicle_Bus_01.model");
	pRcsMgr->Add_ResourcePath("Zero_Vehicle_Bus_01.mat",
		"../../DemoResource/static/Zero_Vehicle_Bus_01.mat");
	Get_Component<CStaticModel>()->Link_Model("Physics_Level", "Zero_Vehicle_Bus_01.model");
	Get_Component<CMaterial>()->Link_Material("Physics_Level", "Zero_Vehicle_Bus_01.mat");

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CDemoFloor::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CDemoFloor::Awake()
{


}

void CDemoFloor::Priority_Update(_float dt)
{
}

void CDemoFloor::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CDemoFloor::Late_Update(_float dt)
{
}

void CDemoFloor::OnCollisionEnter()
{
}

void CDemoFloor::OnCollisionStay()
{
}

void CDemoFloor::OnCollisionExit()
{
}

void CDemoFloor::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add")) {
		CGameObject* DemoFloor = Builder::Create_Object({ "Physics_Level" ,"Proto_GameObject_DemoFloor" })
			.Position({ 0,0,0 })
			.Build("Demo_Floor");
		Get_Component<CObjectContainer>()->Add_Child(DemoFloor, false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

CDemoFloor* CDemoFloor::Create()
{
	CDemoFloor* instance = new CDemoFloor();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoFloor");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoFloor::Clone(INIT_DESC* pArg)
{
	CDemoFloor* instance = new CDemoFloor(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoFloor");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoFloor::Free()
{
	__super::Free();
}
