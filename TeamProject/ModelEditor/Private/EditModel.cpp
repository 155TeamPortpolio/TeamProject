#include "pch.h"
#include "EditModel.h"
#include "AI_STModel.h"
#include "AI_SKModel.h"
#include "SkeletalModel.h"
#include "StaticModel.h"
#include "Material.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "AI_Material.h"
#include "AIMaterial.h"
#include "AIAnimator3D.h"
#include "AIModelData.h"
#include "DebugRender.h"
#include "ModelEditor_BoneData.h"
#include "Mesh.h"

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

	return S_OK;
}

HRESULT CEditModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CEditModel::Awake()
{
	Add_Component<CDebugRender>();
}

void CEditModel::Priority_Update(_float dt)
{
}

void CEditModel::Update(_float dt)
{
	if(nullptr != Get_Component<CAnimator3D>())
		Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CEditModel::Late_Update(_float dt)
{
}

void CEditModel::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 4 + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::SeparatorText("Model Load & Save");
	ImGui::BeginChild("##Loaded OBJECT BTN", ImVec2{0, childHeight}, true);
	static const char* kModes[] = {"Auto", "Forced Static", "Forced Skeletal"};
	int cur = static_cast<int>(m_eMode);
	ImGui::TextUnformatted("Import Mode");
	ImGui::SetNextItemWidth(170.f);
	if (ImGui::Combo("##ImportMode", &cur, kModes, IM_ARRAYSIZE(kModes)))
		m_eMode = static_cast<MODEL_IMPORT_MODE>(cur);

	if (ImGui::Button("Model Load")) {
		string path = Helper::OpenFile_Dialogue();
		if(!path.empty())
			Load_AIScene(path);
	}

	ImGui::SameLine();

	if (ImGui::Button("Model Save")) {
		if (nullptr != Get_Component<CModel>())
			Save_AIScene();
	}
	if (ImGui::Button("Export BoneInfo")) {
		if (nullptr != Get_Component<CModel>())
			ExportBoneInfo();
	}
	ImGui::EndChild();

	ImGui::SeparatorText("Object State");
	ImGui::BeginChild("##ObjectState", ImVec2{0, textLineHeight *2}, true);

	bool alive = Is_Alive();
	if (ImGui::Checkbox("Alive", &alive))
		Set_Alive(alive);

	ImGui::EndChild();

	ImGui::SeparatorText("Position Reset");
	ImGui::BeginChild("##Reset", ImVec2{0,  textLineHeight * 2 }, true);

	if (ImGui::Button("Reset")) {
		Get_Component<CTransform>()->TranslateMatrix(XMMatrixRotationY(XMConvertToRadians(180.f)));
	}

	ImGui::EndChild();

	Get_Component<CTransform>()->Render_GUI();
	if (CAI_SKModel* pSkModel = Get_Component<CAI_SKModel>()) {
		pSkModel->Render_GUI();
	}
	if (CAI_STModel* pStModel = Get_Component<CAI_STModel>()) {
		pStModel->Render_GUI();
	}
	if (CAI_Material* pMaterial = Get_Component<CAI_Material>()) {
		pMaterial->Render_GUI();
	}
	if (CAnimator3D* pAnim = Get_Component<CAnimator3D>()) {
		pAnim->Render_GUI();
	}
}

HRESULT CEditModel::Load_AIScene(const string& filePath)
{
	Clear_Models();
	//m_Importer.FreeScene();
	m_pAIScene = nullptr;

	_uint basFlag =
		aiProcess_ConvertToLeftHanded |
		aiProcessPreset_TargetRealtime_Fast | aiProcess_Triangulate| aiProcess_JoinIdenticalVertices;

	/*메쉬 병합 플래그 끄게*/
	basFlag &= ~(aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

	m_pAIScene = m_Importer.ReadFile(filePath.c_str(), basFlag);
	if (!m_pAIScene)
		return E_FAIL;

	// 2)LOD 노드 탐색

	auto HasLODNodes = [](const aiNode* root) -> bool
		{
			if (!root) return false;

			function<bool(const aiNode*)> recursvieNodeFind = 
				[&](const aiNode* node) -> bool
				{
					if (!node) return false;

					const char* nodeName = node->mName.C_Str();
					if (nodeName && *nodeName)
					{
						string name = nodeName;
						if (name.find("LOD") != string::npos || name.find("lod") !=string::npos)
							return true;
					}

					for (_int i = 0; i < node->mNumChildren; ++i)
						if (recursvieNodeFind(node->mChildren[i]))
							return true;

					return false;
				};

			return recursvieNodeFind(root);
		};

	/*안에 노드 중에 LOD가 하나라도 있나?]*/
	const _bool hasLOD = HasLODNodes(m_pAIScene->mRootNode);
	_bool hasBones = HasBones();

	/*LOD 없고 별거 없으면 그냥 원래 로직대로 로드*/
	if (m_eMode == MODEL_IMPORT_MODE::AUTO && !hasBones && !hasLOD)
	{
		//m_Importer.FreeScene();
		m_pAIScene = nullptr;

		m_pAIScene = m_Importer.ReadFile(filePath.c_str(), basFlag| aiProcess_PreTransformVertices);
		if (!m_pAIScene)
			return E_FAIL;
		hasBones = false;
	}

	if (nullptr == m_pAIScene)
		return E_FAIL;

	Get_Component<CTransform>()->TranslateMatrix(XMMatrixRotationY(XMConvertToRadians(180.f)));

	const _bool isSkeletal = HasBones();
	string fileName = Helper::GetFileNameWithOutExtension(filePath);

	CAI_Material* pMaterial = CAI_Material::Create();
	pMaterial->Set_Owner(this);
	m_Components.emplace(type_index(typeid(CMaterial)), pMaterial);
	m_Components.emplace(type_index(typeid(CAI_Material)), pMaterial);
	Safe_AddRef(pMaterial);

	_uint NumMaterial = m_pAIScene->mNumMaterials;
	pMaterial->Load_Material(NumMaterial, m_pAIScene->mMaterials, filePath);

	if (isSkeletal)
	{
		CAI_SKModel* skeletal = CAI_SKModel::Create();
		m_Components.emplace(type_index(typeid(CSkeletalModel)), skeletal);
		m_Components.emplace(type_index(typeid(CModel)), skeletal);
		m_Components.emplace(type_index(typeid(CAI_SKModel)), skeletal);
		Safe_AddRef(skeletal);
		Safe_AddRef(skeletal);

		skeletal->Load_AIModel(m_pAIScene, fileName);
		skeletal->Set_Owner(this);
		pMaterial->LinkShader("VTX_SkinMesh.hlsl");

		if (m_pAIScene->HasAnimations())
		{
			auto Animator3D = CAIAnimator3D::Create(m_pAIScene, skeletal->Get_AIModelData());
			m_Components.emplace(type_index(typeid(CAnimator3D)), Animator3D);
			Animator3D->Set_Owner(this);
			Animator3D->Link_DynamicBone();
		}
	}
	else
	{
		CAI_STModel* staticModel = CAI_STModel::Create();
		m_Components.emplace(type_index(typeid(CStaticModel)), staticModel);
		m_Components.emplace(type_index(typeid(CModel)), staticModel);
		m_Components.emplace(type_index(typeid(CAI_STModel)), staticModel);
		Safe_AddRef(staticModel);
		Safe_AddRef(staticModel); 

		staticModel->Load_AIModel(m_pAIScene, fileName);
		staticModel->Set_Owner(this);
		pMaterial->LinkShader("VTX_Mesh.hlsl");
	}

	m_InstanceName = fileName;

	return S_OK;
}

HRESULT CEditModel::Save_AIScene()
{
	HRESULT hr = {};
	string SavePath = Helper::OpenFolder_Dialogue() + "\\";
	_matrix PreTransform = XMLoadFloat4x4(Get_Component<CTransform>()->Get_WorldMatrix_Ptr());

	if (HasBones()) {
		CAI_SKModel* pModel = dynamic_cast<CAI_SKModel*>(Get_Component<CModel>());
		hr = pModel->Save_Model(SavePath, PreTransform);

		if (m_pAIScene->HasAnimations()) {
			CAIAnimator3D* pAnimator3D = static_cast<CAIAnimator3D*>(Get_Component<CAnimator3D>());
			hr = pAnimator3D->Save_Animation(SavePath, Get_WorldMatrix());
		}
	}
	else {
		CAI_STModel* pModel = dynamic_cast<CAI_STModel*>(Get_Component<CStaticModel>());
		hr = pModel->Save_Model(SavePath, PreTransform);
	}

	CAI_Material* pMaterial = dynamic_cast<CAI_Material*>(Get_Component<CMaterial>());
	hr = pMaterial->Save_Material(SavePath);

	return S_OK;
}

_bool CEditModel::HasBones()
{
	if (nullptr == m_pAIScene)
		return false;

	if(m_eMode == MODEL_IMPORT_MODE::AUTO){
		for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
		{
			if (m_pAIScene->mMeshes[i]->HasBones())
				return true;
		}

		return false;
	}
	else if (m_eMode == MODEL_IMPORT_MODE::FORCED_STATIC) {
		return false;
	}
	else {
		return true;
	}
}

HRESULT CEditModel::ExportBoneInfo()
{
	BONE_DATA_HEADER header = {};

	header.TagDataFormat = "BoneData";
	header.TagModel = m_InstanceName;

	CAIModelData* pModelData = { nullptr };

	if (HasBones()) {
		CAI_SKModel* pModel = dynamic_cast<CAI_SKModel*>(Get_Component<CModel>());
		pModelData = pModel->Get_AIModelData();
		if (nullptr == pModelData)
			return E_FAIL;
		
	}
	else {
		CAI_STModel* pModel = dynamic_cast<CAI_STModel*>(Get_Component<CStaticModel>());
		pModelData = pModel->Get_AIModelData();
		if (nullptr == pModelData)
			return E_FAIL;
	}



	_uint IBoneCount = pModelData->Get_BoneCount();
	auto pBoneNames = pModelData->Get_BoneNames();


	pModelData->Rake_SkeletonInfo(&header);


	string filename = m_InstanceName + ".BoneData";
	string path = Helper::SaveFileDialogByWinAPI(filename, ".json");

	Helper::SaveJson<BONE_DATA_HEADER>(header, path);

	

	return S_OK;
}

void CEditModel::Clear_Models()
{
	Remove_Component<CAI_STModel>();
	Remove_Component<CAI_SKModel>();
	Remove_Component<CSkeletalModel>();
	Remove_Component<CStaticModel>();
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
	//m_Importer.FreeScene();
	//m_pAIScene = nullptr;
}
