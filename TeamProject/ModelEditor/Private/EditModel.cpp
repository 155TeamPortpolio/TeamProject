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

	ImGui::SeparatorText("Model Load & Save");
	ImGui::BeginChild("##Loaded OBJECT BTN", ImVec2{ 0, childHeight }, true);

	if (ImGui::Button("Model Load")) {
		string path = Helper::OpenFile_Dialogue();
		Load_AIScene(path);
	}

	ImGui::SameLine();

	if (ImGui::Button("Model Save")) {
		Save_AIScene();
	}
	ImGui::EndChild();

	__super::Render_GUI();
}

HRESULT CEditModel::Load_AIScene(const string& filePath)
{
	Clear_Models();
	m_Importer.FreeScene();

	unsigned int iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	m_pAIScene = m_Importer.ReadFile(filePath.c_str(), iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	_bool isSkeletal = HasBones();

	string fileName = Helper::GetFileNameWithOutExtension(filePath);
	CAI_Material* pMaterial = CAI_Material::Create();
	pMaterial->Set_Owner(this);
	m_Components.emplace(type_index(typeid(CMaterial)), pMaterial);
	_uint NumMaterial = m_pAIScene->mNumMaterials;
	pMaterial->Load_Material(NumMaterial, m_pAIScene->mMaterials, filePath);

	if (isSkeletal) {
		auto skeletal = CAI_SKModel::Create();
		m_Components.emplace(type_index(typeid(CAI_SKModel)), skeletal);
		m_Components.emplace(type_index(typeid(CModel)), skeletal);
		Safe_AddRef(skeletal);
		skeletal->Load_AIModel(m_pAIScene, fileName);
		pMaterial->LinkShader("VTX_SkinMesh.hlsl");
	}
	else {
		auto staticModel = CAI_STModel::Create();
		m_Components.emplace(type_index(typeid(CAI_STModel)), staticModel);
		m_Components.emplace(type_index(typeid(CModel)), staticModel);
		Safe_AddRef(staticModel);
		staticModel->Load_AIModel(m_pAIScene, fileName);
		pMaterial->LinkShader("VTX_Mesh.hlsl");
	}
}

HRESULT CEditModel::Save_AIScene()
{
	HRESULT hr = {};

	if (HasBones()) {
		CAI_SKModel* pModel = dynamic_cast<CAI_SKModel*>(Get_Component<CModel>());
		hr = pModel->Save_Model();
	}
	else {
		CAI_STModel* pModel = dynamic_cast<CAI_STModel*>(Get_Component<CStaticModel>());
		hr = pModel->Save_Model();
	}

	CAI_Material* pMaterial = dynamic_cast<CAI_Material*>(Get_Component<CMaterial>());
	hr = pMaterial->Save_Material();

	return S_OK;
}

_bool CEditModel::HasBones()
{
	if (nullptr == m_pAIScene)
		return false;

	for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
	{
		if (m_pAIScene->mMeshes[i]->HasBones())
			return true;
	}

	return false;
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
	m_Importer.FreeScene();
}