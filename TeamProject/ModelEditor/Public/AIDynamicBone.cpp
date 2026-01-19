#include "pch.h"
#include "AIDynamicBone.h"
#include "AISkeleton.h"
#include "GameInstance.h"

CAIDynamicBone::CAIDynamicBone()
	:CDynamicBone()
{
}

HRESULT CAIDynamicBone::Initialize(CAISkeleton* pSkeleton)
{
	if (nullptr == pSkeleton)
		return E_FAIL;

	m_pSkeleton = pSkeleton;
	vector<_float4x4> transformation;
	vector<_float4x4> combined;

	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	transformation.resize(m_pSkeleton->Get_BoneCount(), IdentityMatrix);
	combined.resize(m_pSkeleton->Get_BoneCount(), IdentityMatrix);

	/*뼈 개수만큼 뼈의 로컬상태를 가져옴*/
	for (size_t i = 0; i < m_pSkeleton->Get_BoneCount(); i++)
	{
		transformation[i] = m_pSkeleton->Get_TransformationMatrix(i);
	}
	/*부모 뼈를 받을 수 있게 기본값으로 초기화*/
	for (size_t i = 0; i < m_pSkeleton->Get_BoneCount(); i++)
	{
		int parent = m_pSkeleton->Get_BoneParentIndex(i);

		if (parent == -1) {
			combined[i] = transformation[i];
		}
		else {
			_matrix ParentCombine = XMLoadFloat4x4(&combined[parent]);
			_matrix MyTransformation = XMLoadFloat4x4(&transformation[i]);
			XMStoreFloat4x4(&combined[i], MyTransformation * ParentCombine);
		}
	}

	m_TPose = combined;
	return S_OK;
}

void CAIDynamicBone::Render_GUI()
{
	if (!ImGui::CollapsingHeader("Dynamic Bone (Chain Groups)", ImGuiTreeNodeFlags_DefaultOpen))
		return;
	
	ImGui::BeginChild("##DB_Main", ImVec2(0, 0), true);
	ImGui::Columns(3, "##DB_Columns", true);
	string boneName = m_Gui.selectedBoneIndex < 0 ? "<None>" : m_pSkeleton->Find_BoneNameByIndex(m_Gui.selectedBoneIndex);
	ImGui::Text(boneName.c_str());
	if (m_Gui.selectedBoneIndex > 0)
		if (ImGui::Button("Add Group", ImVec2(120.0f, 0)))
			Create_Chain(m_Gui.selectedBoneIndex);

	//=========================================================
	ImGui::BeginChild("##DB_CreateGroup", ImVec2(0, 0), true);
	ImGui::Text("Bone List");
	ImGui::Separator();

	auto& names = m_pSkeleton->Get_BoneNames();
	for (int nameIndex = 0; nameIndex < (int)names.size(); ++nameIndex)
	{
		const string& name = names[nameIndex];
		if (!FilterBone(name)) continue;
		ImGui::PushID(nameIndex);
		const bool isSelected = (m_Gui.selectedBoneIndex == nameIndex);
		if (ImGui::Selectable(name.c_str(), isSelected))
		{
			m_Gui.selectedBoneIndex = nameIndex; 
			CGameInstance::GetInstance()->Get_GUISystem()->Set_Bone(m_Gui.selectedBoneIndex);
		}
		ImGui::PopID();
	}

	ImGui::EndChild();
	ImGui::NextColumn();

	//=========================================================
	ImGui::BeginChild("##DB_Left", ImVec2(0, 0), false);
	ImGui::Text("Groups (%d)", (int)m_ChainGroups.size());
	ImGui::Separator();
	for (int groupIndex = 0; groupIndex < (int)m_ChainGroups.size(); ++groupIndex)
	{
		auto& group = m_ChainGroups[groupIndex];
		ImGui::PushID(groupIndex);
		const bool selected = (m_Gui.selectedGroupIndex == groupIndex);
		string header = to_string(group.AnchorBoneIndex) + "  [Chains: " + to_string(group.Chains.size()) + "]";
		if (ImGui::Selectable(header.c_str(), selected))
		{
			m_Gui.selectedGroupIndex = groupIndex;
			m_Gui.selectedChainIndex = -1;
			m_Gui.selectedBoneIndex = -1;
		}

		if (selected)
		{
			ImGui::Indent(16.0f);
			for (int chainIndex = 0; chainIndex < (int)group.Chains.size(); ++chainIndex)
			{
				const bool chainSelected = (m_Gui.selectedChainIndex == chainIndex);

				int nodeCount = (int)group.Chains[chainIndex].Nodes.size();
				string chainLabel = "Chain " + to_string(chainIndex) + "  (Nodes: " + to_string(nodeCount) + ")";

				if (ImGui::Selectable(chainLabel.c_str(), chainSelected))
					m_Gui.selectedChainIndex = chainIndex;
			}
			ImGui::Unindent(16.0f);
		}

		ImGui::PopID();
	}

	ImGui::EndChild();
	ImGui::NextColumn();

	//=========================================================
	
	ImGui::BeginChild("##DB_Right", ImVec2(0, 0), false);

	if (m_Gui.selectedGroupIndex >= 0 && m_Gui.selectedGroupIndex < (int)m_ChainGroups.size())
	{
		auto& group = m_ChainGroups[m_Gui.selectedGroupIndex];

		ImGui::Text("Selected Group: %s", to_string(group.AnchorBoneIndex).c_str());
		ImGui::Text("AnchorBoneIndex: %d", (int)group.AnchorBoneIndex);
		ImGui::Separator();

		// --- 그룹 파라미터 ---
		ImGui::Text("Chain Param");

		ImGui::SetNextItemWidth(240.0f);
		ImGui::SliderFloat("Inert", &group.ChainParam.Inert, 0.0f, 1.0f);

		ImGui::SetNextItemWidth(240.0f);
		ImGui::SliderFloat("Damping", &group.ChainParam.Damping, 0.0f, 1.0f);

		ImGui::SetNextItemWidth(240.0f);
		ImGui::SliderFloat("Elasticity", &group.ChainParam.Elasticity, 0.0f, 1.0f);

		ImGui::SetNextItemWidth(240.0f);
		ImGui::SliderFloat("Stiffness", &group.ChainParam.Stiffness, 0.0f, 1.0f);

		ImGui::SetNextItemWidth(240.0f);
		ImGui::SliderFloat("GravityScale", &group.ChainParam.GravityScale, 0.0f, 3.0f);

		ImGui::Separator();

		if (ImGui::Button("Delete Group", ImVec2(140.0f, 0)))
		{
			RemoveGroup(m_Gui.selectedGroupIndex);
			m_Gui.selectedGroupIndex = -1;
			m_Gui.selectedChainIndex = -1;
			m_Gui.selectedBoneIndex = -1;

			ImGui::EndChild();
			ImGui::Columns(1);
			ImGui::EndChild();
			return;
		}

		// --- 선택된 체인 디버그/삭제 ---
		ImGui::Separator();
		if (m_Gui.selectedChainIndex >= 0 && m_Gui.selectedChainIndex < (int)group.Chains.size())
		{
			auto& chain = group.Chains[m_Gui.selectedChainIndex];

			ImGui::Text("Selected Chain: %d", m_Gui.selectedChainIndex);
			ImGui::Text("Node Count: %d", (int)chain.Nodes.size());

			if (ImGui::Button("Delete Chain", ImVec2(140.0f, 0)))
			{
				RemoveChain(m_Gui.selectedGroupIndex, m_Gui.selectedChainIndex);
				m_Gui.selectedChainIndex = -1;
			}

			ImGui::Separator();
			ImGui::Text("Nodes (preview)");
			int previewCount = (int)chain.Nodes.size();
			if (previewCount > 16) previewCount = 16;
			for (int nodeIndex = 0; nodeIndex < previewCount; ++nodeIndex)
			{
				const auto& node = chain.Nodes[nodeIndex];
				ImGui::BulletText("i=%d bone=%d parent=%d len=%.3f dir(%.2f %.2f %.2f)",
					nodeIndex,
					node.BoneIndex,
					node.ParentIndex,
					node.fLength,
					node.RestLocalDir.x, 
					node.RestLocalDir.y, 
					node.RestLocalDir.z
				);
			}
			if ((int)chain.Nodes.size() > previewCount)
				ImGui::TextDisabled("... (%d more)", (int)chain.Nodes.size() - previewCount);
		}
	}
	else
	{
		ImGui::TextDisabled("Select a group from the left list.");
	}

	ImGui::EndChild();
	ImGui::Columns(1);
	ImGui::EndChild();
}

_bool CAIDynamicBone::FilterBone(const string& name)
{
	if (m_Gui.boneFilter.empty()) return true;
	if (name.empty()) return false;
	return name.find(m_Gui.boneFilter) != string::npos;
}

/* 체인을 새로 생성하는 함수 */
HRESULT CAIDynamicBone::Create_Chain(_int AnchorIndex)
{
	if (-1 == AnchorIndex) return E_FAIL;

	for (auto Group : m_ChainGroups) {
		if (AnchorIndex == Group.AnchorBoneIndex)
			return S_OK;
	}

	DYNAMIC_CHAIN_GROUP Group;
	Group.AnchorBoneIndex = AnchorIndex;

	vector<_int> Index;
	Index.push_back(AnchorIndex);

	Create_Node(Index, Group);
	m_ChainGroups.push_back(Group);
	m_Gui.selectedBoneIndex = -1;
	return S_OK;
}

void CAIDynamicBone::Create_Node(vector<_int> Indices, DYNAMIC_CHAIN_GROUP& ChineGroup)
{

	const _int iBoneCount = m_pSkeleton->Get_BoneCount();
	_int iParentIndex = Indices.back();
	vector<_int> Childs;

	for (_int i = iParentIndex + 1; i < iBoneCount; ++i) {
		if (iParentIndex == m_pSkeleton->Get_BoneParentIndex(i))
			Childs.push_back(i);
	}

	if (Childs.empty()) {
		DYNAMIC_CHAIN chain;
		for (_int i = 1; i < Indices.size(); i++) {
			DYNAMIC_NODE Node{};
			Node.ParentIndex = Indices[i - 1];
			Node.BoneIndex = Indices[i];

			Matrix ParentMat = m_TPose[Node.ParentIndex];
			Matrix NodeMat = m_TPose[Node.BoneIndex];

			Node.fLength = (NodeMat.Translation() - ParentMat.Translation()).Length();

			Vector3 WorldDir = NodeMat.Translation() - ParentMat.Translation();
			Matrix parentRot = ParentMat;
			parentRot.Translation(Vector3::Zero);
			Vector3 LocalDir = Vector3::TransformNormal(WorldDir, parentRot.Invert());

			Node.RestLocalDir = LocalDir;
			Node.RestLocalDir.Normalize();

			chain.Nodes.push_back(Node);
		}
		ChineGroup.Chains.push_back(chain);
		return;
	}

	for (_int i = 0; i < Childs.size(); i++) {
		vector<_int> newIndices = Indices;
		newIndices.push_back(Childs[i]);
		Create_Node(newIndices, ChineGroup);
	}
}
void CAIDynamicBone::RemoveGroup(int groupIndex)
{
	if (groupIndex < 0 || groupIndex >= (int)m_ChainGroups.size())
		return;

	m_ChainGroups.erase(m_ChainGroups.begin() + groupIndex);

	if (m_Gui.selectedGroupIndex == groupIndex)
	{
		m_Gui.selectedGroupIndex = -1;
		m_Gui.selectedChainIndex = -1;
	}
	else if (m_Gui.selectedGroupIndex > groupIndex)
	{
		--m_Gui.selectedGroupIndex;
	}

	m_Gui.selectedChainIndex = -1;
}

void CAIDynamicBone::RemoveChain(int groupIndex, int chainIndex)
{
	if (groupIndex < 0 || groupIndex >= (int)m_ChainGroups.size())
		return;

	auto& group = m_ChainGroups[groupIndex];

	if (chainIndex < 0 || chainIndex >= (int)group.Chains.size())
		return;

	group.Chains.erase(group.Chains.begin() + chainIndex);

	if (m_Gui.selectedGroupIndex != groupIndex)
		return;

	if (m_Gui.selectedChainIndex == chainIndex)
	{
		m_Gui.selectedChainIndex = -1;
	}
	else if (m_Gui.selectedChainIndex > chainIndex)
	{
		--m_Gui.selectedChainIndex;
	}
}

void CAIDynamicBone::Save_File(ofstream& ofs)
{
	_uint count = m_ChainGroups.size();
	ofs.write(reinterpret_cast<const char*>(&count), sizeof(_uint));

	for (_uint i = 0; i < count; i++) {
		ofs.write(reinterpret_cast<const char*>(&m_ChainGroups[i]), sizeof(DYNAMIC_CHAIN_GROUP));
	}
}

CAIDynamicBone* CAIDynamicBone::Create( CAISkeleton* pSkeleton)
{
	CAIDynamicBone* instance = new CAIDynamicBone();

	if (FAILED(instance->Initialize(pSkeleton))) {
		Safe_Release(instance);
	}

	return instance;
}
void CAIDynamicBone::Free()
{
	__super::Free();
}