#include "pch.h"
#include "DemoShaderAnimModel.h"
#include "SkeletalModel.h"
#include "Material.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

CDemoShaderAnimModel::CDemoShaderAnimModel()
{
}

CDemoShaderAnimModel::CDemoShaderAnimModel(const CDemoShaderAnimModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDemoShaderAnimModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CDemoShaderAnimModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);


	return S_OK;
}

void CDemoShaderAnimModel::Awake()
{
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	/*파일명과 키값은 일치*/
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");

	Get_Component<CModel>()->Link_Model("Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CMaterial>()->Link_Material("Demo_Level", "Bangboo_Sharkboo_NPC (merge).mat");
}

void CDemoShaderAnimModel::Priority_Update(_float dt)
{
}

void CDemoShaderAnimModel::Update(_float dt)
{
}

void CDemoShaderAnimModel::Late_Update(_float dt)
{
}

void CDemoShaderAnimModel::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add")) {
		CGameObject* DemoModel = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
			.Position({ 0,0,0 })
			.Build("Demo_Model");
		Get_Component<CObjectContainer>()->Add_Child(DemoModel, false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

CDemoShaderAnimModel* CDemoShaderAnimModel::Create()
{
	CDemoShaderAnimModel* instance = new CDemoShaderAnimModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoShaderAnimModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoShaderAnimModel::Clone(INIT_DESC* pArg)
{
	CDemoShaderAnimModel* instance = new CDemoShaderAnimModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoShaderAnimModel");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoShaderAnimModel::Free()
{
	__super::Free();
}
