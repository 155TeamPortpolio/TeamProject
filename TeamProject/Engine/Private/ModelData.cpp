#include "Engine_Defines.h"
#include "ModelData.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Helper_Func.h"
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
		string keyLower = Helper::ToLower(newMesh->Get_Key());
		if (keyLower.find("proxy") != string::npos)
			m_ProxyMarked.push_back(m_Meshes.size() - 1);	
		if (keyLower.find("eff") != string::npos)
			m_ProxyMarked.push_back(m_Meshes.size() - 1);
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

_bool CModelData::Get_RiggedData(HumanoidRigData& outData) {
	 return m_pSkeleton->Get_RiggedData(outData); 
}
_uint CModelData::Get_MaterialIndex(_uint meshIndex)
{
	return m_Meshes[meshIndex]->Get_MaterialIndex();
}

_uint CModelData::Get_BoneCount()
{
	if (!m_pSkeleton)
		return 0;
	return m_pSkeleton->Get_BoneCount();
}

_int CModelData::Find_BoneIndexByName(const string& BoneName)
{
	if (!m_pSkeleton)
		return -1;
	return m_pSkeleton->Find_BoneIndexByName(BoneName);
}

const string& CModelData::Find_BoneNameByIndex(_uint BoneIndex)
{
	if (!m_pSkeleton)
		return "";
	return m_pSkeleton->Find_BoneNameByIndex(BoneIndex);
}

_bool CModelData::isRootBone(_uint BoneIndex)
{
	if (m_pSkeleton->Get_BoneParentIndex(BoneIndex) == -1)
		return true;
	else
		return false;
}


_float4x4 CModelData::Get_Offset(_uint meshIndex, _uint boneIndex)
{
	return m_Meshes[meshIndex]->Get_MeshOffset(boneIndex);
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
	if (m_pSkeleton)
	{
		if (ImGui::Button("Bones Tab"))
			isGui_BoneTabOpen = !isGui_BoneTabOpen;

		if (isGui_BoneTabOpen)
		{
			string boneCount = "Bone : " + to_string(m_pSkeleton->Get_BoneCount());
			ImGui::Text(boneCount.c_str());
			ImGui::SetNextWindowSize(ImVec2(500, 400));

			if (ImGui::Begin("SkeletonBones", &isGui_BoneTabOpen, ImGuiWindowFlags_NoCollapse))
				m_pSkeleton->Render_GUI();
			ImGui::End();
		}
	}

	if (ImGui::Button("Material Stats"))
	{
		isGui_MaterialStatsOpen = !isGui_MaterialStatsOpen;
		if (isGui_MaterialStatsOpen)
			m_cachedMaterialUsage = BuildMaterialUsageTable(true);
	}

	if (isGui_MaterialStatsOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 420), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("MaterialUsage", &isGui_MaterialStatsOpen, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::Text("Mesh Count: %d", (int)m_Meshes.size());
			ImGui::Separator();

			if (ImGui::Button("Refresh"))
				m_cachedMaterialUsage = BuildMaterialUsageTable(true);

			ImGui::Separator();

			if (ImGui::BeginTable("##MatUsageTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
			{
				ImGui::TableSetupColumn("MaterialIndex", ImGuiTableColumnFlags_WidthFixed, 120.f);
				ImGui::TableSetupColumn("Meshes", ImGuiTableColumnFlags_WidthFixed, 70.f);
				ImGui::TableSetupColumn("Mesh Indices", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();

				for (const auto& row : m_cachedMaterialUsage)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%u", row.materialIndex);

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%u", row.meshCount);

					ImGui::TableSetColumnIndex(2);

					string label = "List##mat_" + to_string(row.materialIndex);
					if (ImGui::TreeNode(label.c_str()))
					{
						for (size_t i = 0; i < row.meshIndices.size(); ++i)
						{
							ImGui::SameLine();
							ImGui::Text("%u", row.meshIndices[i]);
							if (i + 1 < row.meshIndices.size())
							{
								ImGui::SameLine();
								ImGui::TextUnformatted(",");
							}
						}
						ImGui::NewLine();
						ImGui::TreePop();
					}
				}

				ImGui::EndTable();
			}
		}
		ImGui::End();
	}
}

vector<MaterialUsageRow> CModelData::BuildMaterialUsageTable(_bool includeMeshIndices) const
{
	unordered_map<_uint, MaterialUsageRow> mapByMaterial;
	mapByMaterial.reserve(m_Meshes.size());

	for (_uint meshIndex = 0; meshIndex < (_uint)m_Meshes.size(); ++meshIndex)
	{
		CMesh* mesh = m_Meshes[meshIndex];
		if (!mesh) continue;

		_uint materialIndex = mesh->Get_MaterialIndex();

		auto found = mapByMaterial.find(materialIndex);
		if (found == mapByMaterial.end())
		{
			MaterialUsageRow row;
			row.materialIndex = materialIndex;
			row.meshCount = 1;
			if (includeMeshIndices)
				row.meshIndices.push_back(meshIndex);

			mapByMaterial.emplace(materialIndex, std::move(row));
		}
		else
		{
			found->second.meshCount += 1;
			if (includeMeshIndices)
				found->second.meshIndices.push_back(meshIndex);
		}
	}

	vector<MaterialUsageRow> rows;
	rows.reserve(mapByMaterial.size());
	for (auto& pair : mapByMaterial)
		rows.push_back(std::move(pair.second));

	// meshCount 내림차순으로 정렬(많이 쓰는 머티리얼이 위로)
	sort(rows.begin(), rows.end(),
		[](const MaterialUsageRow& leftRow, const MaterialUsageRow& rightRow)
		{
			if (leftRow.meshCount != rightRow.meshCount)
				return leftRow.meshCount > rightRow.meshCount;
			return leftRow.materialIndex < rightRow.materialIndex;
		});

	return rows;
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

void CModelData::Get_AffectBoneIndices(vector<_int>& outvec, _int StartBoneIndex)
{
	outvec.clear();
	m_pSkeleton->Get_AffectBoneIndices(outvec, StartBoneIndex);
}

vector<DYNAMIC_CHAIN_GROUP> CModelData::Get_ChaingGroups()
{
	return m_pSkeleton->Get_ChainGroups();
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
