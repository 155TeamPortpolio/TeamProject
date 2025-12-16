#include "pch.h"
#include "DemoShaderModel.h"
#include "SkeletalModel.h"
#include "Material.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

CDemoShaderModel::CDemoShaderModel()
{
}

CDemoShaderModel::CDemoShaderModel(const CDemoShaderModel& rhs)
    :CGameObject(rhs)
{
}

HRESULT CDemoShaderModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CDemoShaderModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);


	return S_OK;
}

void CDemoShaderModel::Awake()
{
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	/*파일명과 키값은 일치*/
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");

	Get_Component<CModel>()->Link_Model("Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CModel>()->Set_RenderType(RENDER_PASS_TYPE::NONLIGHT_OPAQUE);
	Get_Component<CMaterial>()->Link_Material("Demo_Level", "Bangboo_Sharkboo_NPC (merge).mat");
}

void CDemoShaderModel::Priority_Update(_float dt)
{
}

void CDemoShaderModel::Update(_float dt)
{
}

void CDemoShaderModel::Late_Update(_float dt)
{
}

void CDemoShaderModel::Render_GUI()
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

CDemoShaderModel* CDemoShaderModel::Create()
{
	CDemoShaderModel* instance = new CDemoShaderModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoShaderModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoShaderModel::Clone(INIT_DESC* pArg)
{
	CDemoShaderModel* instance = new CDemoShaderModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoShaderModel");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoShaderModel::Free()
{
	__super::Free();
}
