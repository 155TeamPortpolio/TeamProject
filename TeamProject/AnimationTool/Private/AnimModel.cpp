#include "pch.h"
#include "AnimModel.h"

#include "Helper_Func.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "AnimationLayout.h"
#include "AnimationClip.h"
#include "Animator3D.h"


CAnimModel::CAnimModel()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAnimModel::CAnimModel(const CAnimModel& rhs)
	:CGameObject(rhs)
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	//Add_Component<CSkeletalModel>();
	//Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CAnimModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	//m_Components.emplace(type_index(typeid()))
	//m_Components.emplace(type_index(typeid(CAnimator3DEX)));

	return S_OK;
}

void CAnimModel::Awake()
{
	//CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("", "");
	//Get_Component<CModel>()->Link_Model("Demo_Level", "");
}

void CAnimModel::Priority_Update(_float dt)
{
}

void CAnimModel::Update(_float dt)
{
	if (nullptr != Get_Component<CAnimator3D>())
		Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CAnimModel::Late_Update(_float dt)
{
}

void CAnimModel::Render_GUI()
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

void CAnimModel::GUI_LoadResource(_float fChildHeight)
{
	ImGui::SeparatorText("Model & Material Load");
	ImGui::BeginChild("##Loaded Data", ImVec2{ 0, fChildHeight }, true);
	if (ImGui::Button("Load Resource")) {
		Load_Resource();
	}
	ImGui::SameLine();
	if (ImGui::Button("Set Meta")) {
		Set_Animator();
	}
	ImGui::EndChild();
}

void CAnimModel::GUI_SetModel(_float fChildHeight)
{
	//Select Model
	ImGui::SeparatorText("Model & Material Set");
	ImGui::BeginChild("##Set Data", ImVec2{ 0, fChildHeight * 2 }, true);
	if (ImGui::BeginCombo("##Model Combo", m_CurModelTag.c_str())) //Model
	{
		if (!m_ModelTags.empty()) {
			for (string ModelTag : m_ModelTags)
			{
				bool selected = (m_CurModelTag == ModelTag);
				if (ImGui::Selectable(ModelTag.c_str(), selected))
				{
					m_CurModelTag = ModelTag;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	//Select Material
	if (ImGui::BeginCombo("##Material Combo", m_CurMaterialTag.c_str())) //Material
	{
		if (!m_MaterialTags.empty()) {
			for (string MatTag : m_MaterialTags)
			{
				bool selected = (m_CurMaterialTag == MatTag);
				if (ImGui::Selectable(MatTag.c_str(), selected))
				{
					m_CurMaterialTag = MatTag;
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
		Set_Model(m_CurModelTag, m_CurMaterialTag);
	}

	ImGui::EndChild();
}

void CAnimModel::Load_Resource()
{
	vector<string> files = Helper::OpenMultiFiles();

	for (auto& path : files) {
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
}

void CAnimModel::Set_Model(string ModelTag, string MaterialTag)
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

void CAnimModel::Set_Animator()
{
	Remove_Component<CAnimator3D>();
	auto ResMgr = m_pGameInstance->Get_ResourceMgr();

	string metaPath = Helper::OpenFile_Dialogue();
	string metaTag = Helper::GetFileNameWithExtension(metaPath);
	ResMgr->Add_ResourcePath(metaTag, metaPath);

	Add_Component<CAnimator3D>();
	Get_Component<CAnimator3D>()->LinkAnimate_Model("AnimationEdit_Level", m_CurModelTag);
	Get_Component<CAnimator3D>()->Link_MetaData("AnimationEdit_Level", metaTag);
}

void CAnimModel::Clear_Model()
{
	Remove_Component<CSkeletalModel>();
	Remove_Component<CModel>();
	Remove_Component<CMaterial>();
}

CAnimModel* CAnimModel::Create()
{
	CAnimModel* instance = new CAnimModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CAnimModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CAnimModel::Clone(INIT_DESC* pArg)
{
	CAnimModel* instance = new CAnimModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CAnimModel");
		Safe_Release(instance);
	}

	return instance;
}

void CAnimModel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}