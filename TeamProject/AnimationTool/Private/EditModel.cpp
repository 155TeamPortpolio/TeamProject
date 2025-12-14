#include "pch.h"
#include "EditModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Helper_Func.h"
#include "GameInstance.h"

CEditModel::CEditModel()
{
}

CEditModel::CEditModel(const CEditModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CEditModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	//Add_Component<CSkeletalModel>();
	//Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CEditModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	//m_Components.emplace(type_index(typeid()))

	return S_OK;
}

void CEditModel::Awake()
{
	//CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("", "");
	//Get_Component<CModel>()->Link_Model("Demo_Level", "");
}

void CEditModel::Priority_Update(_float dt)
{
}

void CEditModel::Update(_float dt)
{
}

void CEditModel::Late_Update(_float dt)
{
}

void CEditModel::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	//Load Resource
	GUI_LoadResource(childHeight);

	//Set Model, Materials
	GUI_SetModel(childHeight);

	__super::Render_GUI();
}

void CEditModel::GUI_LoadResource(_float fChildHeight)
{
	ImGui::SeparatorText("Model & Material Load");
	ImGui::BeginChild("##Loaded Data", ImVec2{ 0, fChildHeight }, true);
	if (ImGui::Button("Load Resource")) {
		Load_Resource();
	}
	ImGui::EndChild();
}

void CEditModel::GUI_SetModel(_float fChildHeight)
{
	//Show Selected Tags
	static string CurModelTag = { "Select Model" };
	static string CurMaterialTag = { "Select Material" };
	//Select Model
	ImGui::SeparatorText("Model & Material Set");
	ImGui::BeginChild("##Set Data", ImVec2{ 0, fChildHeight * 2 }, true);
	if (ImGui::BeginCombo("##Model Combo", CurModelTag.c_str())) //Model
	{
		if (!m_ModelTags.empty()) {
			for (string ModelTag : m_ModelTags)
			{
				bool selected = (CurModelTag == ModelTag);
				if (ImGui::Selectable(ModelTag.c_str(), selected))
				{
					CurModelTag = ModelTag;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	//Select Material
	if (ImGui::BeginCombo("##Material Combo", CurMaterialTag.c_str())) //Material
	{
		if (!m_MaterialTags.empty()) {
			for (string MatTag : m_MaterialTags)
			{
				bool selected = (CurMaterialTag == MatTag);
				if (ImGui::Selectable(MatTag.c_str(), selected))
				{
					CurMaterialTag = MatTag;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	//Set Models;
	ImGui::SameLine();
	if (ImGui::Button("Set")) {
		Set_Model(CurModelTag, CurMaterialTag);
	}

	ImGui::EndChild();
}

void CEditModel::Load_Resource()
{
	string path = Helper::OpenFile_Dialogue();
	string ext = std::filesystem::path(path).extension().string();
	string name = std::filesystem::path(path).stem().string() + ext;

	if (".model" == ext) {
		if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
			m_ModelTags.insert(name);
		}
	}
	else if (".mat" == ext) {
		if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
			m_MaterialTags.insert(name);
		}
	}
}

void CEditModel::Set_Model(string ModelTag, string MaterialTag)
{
	Clear_Model();

	string modelName = Helper::GetFileNameWithOutExtension(ModelTag);
	string matName = Helper::GetFileNameWithOutExtension(MaterialTag);

	if (modelName != matName) {
		OutputDebugStringA("Not Same Name");
		return;
	}

	Add_Component<CSkeletalModel>();
	Get_Component<CSkeletalModel>()->Link_Model("AnimationEdit_Level", ModelTag);

	Add_Component<CMaterial>();
	Get_Component<CMaterial>()->Link_Material("AnimationEdit_Level", MaterialTag);
}

void CEditModel::Clear_Model()
{
	Remove_Component<CSkeletalModel>();
	Remove_Component<CModel>();
	Remove_Component<CMaterial>();
}

CEditModel* CEditModel::Create()
{
	CEditModel* instance = new CEditModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CEditModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CEditModel::Clone(INIT_DESC* pArg)
{
	CEditModel* instance = new CEditModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CEditModel");
		Safe_Release(instance);
	}

	return instance;
}

void CEditModel::Free()
{
	__super::Free();
}