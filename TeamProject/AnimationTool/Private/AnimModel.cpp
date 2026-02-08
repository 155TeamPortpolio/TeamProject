#include "pch.h"
#include "AnimModel.h"

#include "Helper_Func.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "AnimationLayout.h"
#include "AnimationClip.h"
#include "Animator3D.h"
#include "Animator3DEX.h"

#include "AnimToolPanel.h"
#include "AudioSource.h"

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
	Add_Component<CAudioSource>();
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
	auto input = CGameInstance::GetInstance()->Get_InputDev();
	if (input->Key_Down(VK_UP))
		m_pTransform->Translate(m_pTransform->Dir(STATE::LOOK) * dt);
	if (input->Key_Down(VK_DOWN))
		m_pTransform->Translate(-m_pTransform->Dir(STATE::LOOK) * dt);
	if (input->Key_Down(VK_RIGHT))
		m_pTransform->Translate(m_pTransform->Dir(STATE::RIGHT) * dt);
	if (input->Key_Down(VK_LEFT))
		m_pTransform->Translate(-m_pTransform->Dir(STATE::RIGHT) * dt);
	if (input->Key_Down(VK_OEM_COMMA)) { // <
		m_fDegree -= 30.f * dt;
		m_pTransform->Rotate(_float3(0.f, XMConvertToRadians(m_fDegree), 0.f));
	}
	if (input->Key_Down(VK_OEM_PERIOD)) { // >
		m_fDegree += 30.f * dt;
		m_pTransform->Rotate(_float3(0.f, XMConvertToRadians(m_fDegree), 0.f));
	}
}

void CAnimModel::Update(_float dt)
{
	if (auto pAnimator = Get_Component<CAnimator3D>()) {
		m_pTransform->Translate(_vector3(pAnimator->Get_RootBoneMoveDelta() * m_fMoveSpeed));
		_quaternion dq = pAnimator->Get_RootBoneQuatDelta(); // 반환 타입이 XMFLOAT4라고 가정
		//m_pTransform->(dq);
		m_pTransform->Add_Quaternion(dq);
	}
}

void CAnimModel::Late_Update(_float dt)
{
}

void CAnimModel::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::DragFloat("MoveSpeed", &m_fMoveSpeed, 0.001f, 0.f, 100.f);

	//Load Resource
	GUI_LoadResource(childHeight*3);

	//Set Model, Materials
	GUI_SetModel(childHeight);

	__super::Render_GUI();
}

void CAnimModel::Set_Panel(CAnimToolPanel* pAnimToolPanel)
{
	m_pAnimToolPanel = pAnimToolPanel;
}

void CAnimModel::GUI_LoadResource(_float fChildHeight)
{
	//if (Get_Component<CAnimator3D>()) {
	//	Matrix mat = Get_Component<CAnimator3D>()->Get_BoneMatrix(CAnimator3D::BoneSpace::WORLD, 230);
	//	ImGui::Text("%.2f\t%.2f\t%.2f\t%.2f", mat._11, mat._12, mat._13, mat._14);
	//	ImGui::Text("%.2f\t%.2f\t%.2f\t%.2f", mat._21, mat._22, mat._23, mat._24);
	//	ImGui::Text("%.2f\t%.2f\t%.2f\t%.2f", mat._31, mat._32, mat._33, mat._34);
	//	ImGui::Text("%.2f\t%.2f\t%.2f\t%.2f", mat._41, mat._42, mat._43, mat._44);
	//}

	ImGui::SeparatorText("Model & Material Load");
	ImGui::BeginChild("##Loaded Data", ImVec2{ 0, fChildHeight }, true);

	if (ImGui::Button("NewModelMat")) {
		Load_NewModelMat();
	}
	ImGui::SameLine();
	if (ImGui::Button("NewMeta")) {
		Load_NewMeta();
	}

	if (ImGui::Button("Once Load")) {
		Load_ModelOnce();
	}

	if (ImGui::Button("Load Resource")) {
		Load_Resource();
	}
	ImGui::SameLine();
	if (ImGui::Button("Set Meta")) {
		Set_Animator();
	}

	if (ImGui::Button("Load Effect")) {
		Load_Resource();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Sound")) {
		Load_Sound();
	}
	ImGui::SameLine();

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

void CAnimModel::Load_NewModelMat()
{
	vector<string> files = Helper::OpenMultiFiles();
	if (files.size() != 2)
		return;

	Clear_Model();
	for (const auto& path : files)
	{
		string ext = std::filesystem::path(path).extension().string();
		string name = std::filesystem::path(path).stem().string() + ext;

		if (".model" == ext) {
			if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
				Add_Component<CSkeletalModel>();
				Get_Component<CSkeletalModel>()->Link_Model("AnimationEdit_Level", name);
				m_CurModelTag = name;
			}
		}
		else if (".mat" == ext) {
			if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
				Add_Component<CMaterial>();
				Get_Component<CMaterial>()->Link_Material("AnimationEdit_Level", name);
				m_CurMaterialTag = name;
			}
		}
	}
}

void CAnimModel::Load_NewMeta()
{
	Remove_Component<CAnimator3DEX>();
	Remove_Component<CAnimator3D>();

	auto ResMgr = m_pGameInstance->Get_ResourceMgr();

	string metaPath = Helper::OpenFile_Dialogue();
	string metaTag = Helper::GetFileNameWithExtension(metaPath);

	if ("" == metaTag || string::npos == metaTag.find("_Meta.json"))
		return;

	ResMgr->Add_ResourcePath(metaTag, metaPath);

	CAnimator3DEX* pInstance = CAnimator3DEX::Create();
	pInstance->Set_Owner(this);

	m_Components.emplace(type_index(typeid(CAnimator3DEX)), pInstance);
	m_Components.emplace(type_index(typeid(CAnimator3D)), pInstance);

	pInstance->LinkAnimate_Model("AnimationEdit_Level", m_CurModelTag);
	pInstance->Link_MetaData("AnimationEdit_Level", metaTag);

	m_pAnimToolPanel->Setting_NewClip();
	m_pAnimToolPanel->Setting_MetaFilePath(metaPath);

	Safe_AddRef(pInstance);
}

void CAnimModel::Load_ModelOnce()
{
	vector<string> files = Helper::OpenMultiFiles();
	if (files.size() != 3)
		return;

	string baseName;
	bool bModel = false, bMat = false, bJson = false;
	

	for (const auto& pathStr : files)
	{
		std::filesystem::path p(pathStr);
		string ext = p.extension().string();
		string stem = p.stem().string();

		if (ext == ".model")
		{
			if (bModel) return;
			bModel = true;
		}
		else if (ext == ".mat")
		{
			if (bMat) return;
			bMat = true;
		}
		else if (ext == ".json")
		{
			if (bJson) return;
			bJson = true;
		}
		else
			return;
	}

	if (!(bModel && bMat && bJson))
		return;

	auto ResMgr = m_pGameInstance->Get_ResourceMgr();

	string Model, Material, Meta;
	string MetaFilePath{};

	for (const auto& path : files)
	{
		string ext = std::filesystem::path(path).extension().string();
		string name = std::filesystem::path(path).stem().string() + ext;

		if (".model" == ext) {
			if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
				Model = name;
			}
		}
		else if (".mat" == ext) {
			if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
				Material = name;
			}
		}
		else if (".json" == ext) {
			if (SUCCEEDED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(name, path))) {
				Meta = name;
				MetaFilePath = path;
			}
		}
	}

	Add_Component<CSkeletalModel>();
	Get_Component<CSkeletalModel>()->Link_Model("AnimationEdit_Level", Model);

	Add_Component<CMaterial>();
	Get_Component<CMaterial>()->Link_Material("AnimationEdit_Level", Material);

	CAnimator3DEX* pInstance = CAnimator3DEX::Create();
	pInstance->Set_Owner(this);

	m_Components.emplace(type_index(typeid(CAnimator3DEX)), pInstance);
	m_Components.emplace(type_index(typeid(CAnimator3D)), pInstance);
	Safe_AddRef(pInstance);

	pInstance->LinkAnimate_Model("AnimationEdit_Level", Model);
	pInstance->Link_MetaData("AnimationEdit_Level", Meta);

	m_pAnimToolPanel->Setting_NewClip();
	m_pAnimToolPanel->Setting_MetaFilePath(MetaFilePath);
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

void CAnimModel::Load_Sound()
{
	string folderPath = Helper::OpenFolder_Dialogue();
	Get_Component<CAudioSource>()->SoundFolder(G_GlobalLevelKey, folderPath);
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
	Remove_Component<CAnimator3DEX>();
	Remove_Component<CAnimator3D>();

	auto ResMgr = m_pGameInstance->Get_ResourceMgr();

	string metaPath = Helper::OpenFile_Dialogue();
	string metaTag = Helper::GetFileNameWithExtension(metaPath);

	if ("" == metaTag || string::npos == metaTag.find("_Meta.json"))
		return;

	ResMgr->Add_ResourcePath(metaTag, metaPath);

	CAnimator3DEX* pInstance = CAnimator3DEX::Create();
	pInstance->Set_Owner(this);

	m_Components.emplace(type_index(typeid(CAnimator3DEX)), pInstance);
	m_Components.emplace(type_index(typeid(CAnimator3D)), pInstance);
	Safe_AddRef(pInstance);
	
	pInstance->LinkAnimate_Model("AnimationEdit_Level", m_CurModelTag);
	pInstance->Link_MetaData("AnimationEdit_Level", metaTag);

	m_pAnimToolPanel->Setting_NewClip();
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
	Remove_Component<CAnimator3DEX>();
}