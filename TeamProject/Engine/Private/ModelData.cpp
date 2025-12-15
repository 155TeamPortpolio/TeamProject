#include "Engine_Defines.h"
#include "ModelData.h"
#include "Mesh.h"
#include "Skeleton.h"

CModelData::CModelData()
{
}

CModelData::~CModelData()
{
}

HRESULT CModelData::Initialize(const string& filePath, ID3D11Device* pDevice)
{

	ifstream ifs(filePath.c_str(), ios::binary);
	if (!ifs.is_open()) {
		MSG_BOX("There is No File. :CModelData ");
		return E_FAIL;
	}

	MODEL_FILE_HEADER fileHeader = {};
	ifs.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

	for (int i = 0; i < fileHeader.MeshCount; ++i) {
		CMesh* newMesh = CMesh::Create(pDevice, ifs, fileHeader.isAnimate? MESH_TYPE::ANIM : MESH_TYPE::NONANIM);
		if (newMesh)
			m_Meshes.push_back(newMesh);
		else
		{
			ifs.close();
			for (auto& mesh : m_Meshes)
				Safe_Release(mesh);
			return E_FAIL;
		}
	}

	if (fileHeader.isAnimate) {
		m_pSkeleton = CSkeleton::Create(ifs);
	}

	ifs.close();

	if (fileHeader.isAnimate) {
		for (auto mesh : m_Meshes) {
			mesh->Create_BoneMinMax(m_pSkeleton);
		}
	}

	for (auto mesh : m_Meshes) {
		_float3 meshMin = mesh->Get_MinVertexLocal();
		_float3 meshMax = mesh->Get_MaxVertexLocal();

		m_vMinLocal.x = min(m_vMinLocal.x, meshMin.x);
		m_vMinLocal.y = min(m_vMinLocal.y, meshMin.y);
		m_vMinLocal.z = min(m_vMinLocal.z, meshMin.z);

		m_vMaxLocal.x = max(m_vMaxLocal.x, meshMax.x);
		m_vMaxLocal.y = max(m_vMaxLocal.y, meshMax.y);
		m_vMaxLocal.z = max(m_vMaxLocal.z, meshMax.z);
	}

	return S_OK;
}

_uint CModelData::Get_MaterialIndex(_uint meshIndex)
{
	return m_Meshes[meshIndex]->Get_MaterialIndex();
}

_uint CModelData::Get_BoneCount()
{
	return m_pSkeleton->Get_BoneCount();
}

_int CModelData::Find_BoneIndexByName(const string& BoneName)
{
	return m_pSkeleton->Find_BoneIndexByName(BoneName);
}

const string& CModelData::Find_BoneNameByIndex(_uint BoneIndex)
{
	return m_pSkeleton->Find_BoneNameByIndex(BoneIndex);
}

_bool CModelData::isRootBone(_uint BoneIndex)
{
	if (m_pSkeleton->Get_BoneParentIndex(BoneIndex) == -1)
		return true;
	else
		return false;
}

const D3D11_INPUT_ELEMENT_DESC* CModelData::Get_ElementDesc(_uint DrawIndex)
{
	return m_Meshes[DrawIndex]->Get_ElementDesc();
}

const _uint CModelData::Get_ElementCount(_uint DrawIndex)
{
	return m_Meshes[DrawIndex]->Get_ElementCount();
}

const string_view CModelData::Get_ElementKey(_uint DrawIndex)
{
	return m_Meshes[DrawIndex]->Get_ElementKey();
}

_int CModelData::Find_MeshIndex(const string& name)
{
	for (size_t i = 0; i < m_Meshes.size(); i++)
	{
		if (m_Meshes[i]->Get_Key().find(name) !=string::npos) {
			return i;
		}
	}
	return -1;
}

void CModelData::Render_GUI()
{
	string meshCount = "Mesh : " + to_string(m_Meshes.size());

	ImGui::Text(meshCount.c_str());
	for (auto& mesh : m_Meshes) {
		mesh->Render_GUI();
	}
	ImGui::Separator();

	if (m_pSkeleton) {
		if (ImGui::Button("Bones Tab")) {
			isGui_BoneTabOpen = !isGui_BoneTabOpen;
		}
		if(isGui_BoneTabOpen){
		string boneCount = "Bone : " + to_string(m_pSkeleton->Get_BoneCount());
		ImGui::Text(boneCount.c_str());
		ImGui::SetNextWindowSize(ImVec2(500, 400));
		
		if (ImGui::Begin("SkeletonBones", &isGui_BoneTabOpen, ImGuiWindowFlags_NoCollapse))
		{
			m_pSkeleton->Render_GUI();
		}
		ImGui::End();
		}
	}

}

HRESULT CModelData::Render_Mesh(ID3D11DeviceContext* pContext, _uint Index)
{
	if (Index >= m_Meshes.size()) return E_FAIL;

	m_Meshes[Index]->Bind_Buffer(pContext);
	m_Meshes[Index]->Render(pContext);

	return S_OK;
}

_matrix CModelData::Get_OffsetMatrix(_uint BoneIndex)
{
	return m_pSkeleton->Get_OffsetMatrix(BoneIndex);
}

const vector<string> CModelData::Get_BoneNames()
{
	return m_pSkeleton->Get_BoneNames();
}

const vector<_int> CModelData::GenerateFollowingIndices(CModelData* pMasterData)
{
	vector<_int> FollowingIndices;
	/*마스터 데이터에서 내 이름과 같은 것이 있으면 넣는다. 없으면 -1이 들어간다*/
	for (string boneName : Get_BoneNames()) {
		_int Index = pMasterData->Find_BoneIndexByName(boneName);
		FollowingIndices.push_back(Index);
	}

	return FollowingIndices;
}

MINMAX_BOX CModelData::Get_LocalBoundingBox()
{
	return MINMAX_BOX{m_vMinLocal,m_vMaxLocal};
}

MINMAX_BOX CModelData::Get_MeshBoundingBox(_uint index)
{
	MINMAX_BOX box = {};
	if (index >= m_Meshes.size()) {
		return box;
	}
	box.vMax = m_Meshes[index]->Get_MaxVertexLocal();
	box.vMin = m_Meshes[index]->Get_MinVertexLocal();
	return box;
}

_int CModelData::Get_BoneParentIndex(_uint i)
{
	return m_pSkeleton->Get_BoneParentIndex(i);
}

_float4x4 CModelData::Get_TransformMatrix(_uint BoneIndex)
{
	return m_pSkeleton->Get_TransformationMatrix(BoneIndex);
}

CModelData* CModelData::Create(const string& filePath, ID3D11Device* pDevice)
{
	CModelData* instance = new CModelData;

	if (FAILED(instance->Initialize(filePath, pDevice))) {
		Safe_Release(instance);
	}
	return instance;
}

void CModelData::Free()
{
	__super::Free();
	if(m_pSkeleton)
		Safe_Release(m_pSkeleton);
	for (auto& mesh : m_Meshes)
		Safe_Release(mesh);
}
