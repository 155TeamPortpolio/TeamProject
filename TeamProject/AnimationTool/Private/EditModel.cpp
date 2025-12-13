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
	//Add_Component<CStaticModel>();
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
	
	ImGui::SeparatorText("Model & Material Load");
	ImGui::BeginChild("##Loaded Data", ImVec2{ 0, childHeight * 2 }, true);
	
	if (ImGui::Button("Load Resource")) {
		Load_Resource();
	}

	if (ImGui::Button("Model Load")) {
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Material Load")) {

	}
	ImGui::EndChild();


	__super::Render_GUI();
}

void CEditModel::Load_Resource()
{
	string path = Helper::OpenFile_Dialogue();
	string name = std::filesystem::path(path).stem().string();
	string ext = std::filesystem::path(path).extension().string();
	
	if (".model" == ext) {
		if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
			m_ModelTag.push_back(name);
		}
	}
	else if (".mat" == ext) {
		if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
			m_ModelTag.push_back(name);
		}
	}
}

void CEditModel::Set_Model()
{
}

void CEditModel::Set_Material()
{
}

void CEditModel::Clear_Model()
{
	Remove_Component<CSkeletalModel>();
	Remove_Component<CModel>();
	Remove_Component<CMaterial>();
}

void CEditModel::Clear_Material()
{
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