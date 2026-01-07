#include "pch.h"
#include "AISkeleton.h"
#include "AIBone.h"
#include "Helper_Func.h"
#include "HumanoidRule.h"
#include "AIDynamicBone.h"

CAISkeleton::CAISkeleton()
{
}

HRESULT CAISkeleton::Initialize(const aiNode* _pAINode)
{
	if (FAILED(Ready_Bones(_pAINode)))
		return E_FAIL;

	m_RiggedData.map.Reset();
	return S_OK;
}

void CAISkeleton::Set_Offset(_uint Index, _float4x4 offset)
{
	if (HasOffset[Index] == false) {
		if (m_Bones[Index]->Get_ParentIndex() == -1) {
			_matrix Offset = XMLoadFloat4x4(&offset);
			XMStoreFloat4x4(&m_OffsetMatrices[Index], Offset);
		}
		else {
			m_OffsetMatrices[Index] = offset;
		}
		HasOffset[Index] = true;
	}
	else {
		_smatrix newOne = (offset);
		_smatrix oldOne = m_OffsetMatrices[Index];
		if (newOne != oldOne)
			m_OffsetMatrices[Index] = offset;
	}
}

void CAISkeleton::Render_GUI()
{
	if (ImGui::Button("Make_Humanoid")) {
		m_isHumanoidTabOpened = !m_isHumanoidTabOpened;
	}
	if (m_isHumanoidTabOpened) {
		Render_Humanoid();
	}

	if (ImGui::Button("Make_DynamicChain ")) {
		m_isDynamicTabOpened = !m_isDynamicTabOpened;
	}
	if (m_isDynamicTabOpened) {
		Render_DynamicChain();
	}
}

void CAISkeleton::Render_Humanoid()
{
	ImGui::SetNextWindowSize(ImVec2(520, 520), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(420, 360), ImVec2(10000, 10000));
	ImGui::Begin("Humanoid Tab", &m_isHumanoidTabOpened);

	_int requiredCount = 0;
	_int mappedRequiredCount = 0;

	for (_int slotIndex = 0; slotIndex < (_int)ENUM(HumanoidBone::Count); ++slotIndex)
	{
		HumanoidBone slot = (HumanoidBone)slotIndex;
		if (!IsRequiredSlot(slot)) continue;
		requiredCount++;
		if (m_RiggedData.map.indexByRole[slotIndex] >= 0)
			mappedRequiredCount++;
	}

	ImGui::InputText("##filterText", &m_BoneFilter);

	float mappedRatio = (requiredCount > 0)
		? (float)mappedRequiredCount / (float)requiredCount
		: 0.0f;

	if (m_RiggedData.IsRigComplete())
		ImGui::TextColored(ImVec4(0.3f, 1.f, 0.3f, 1.f), "Rig Status: COMPLETE");
	else
		ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), "Rig Status: INCOMPLETE");

	ImGui::Text("Required: %d / %d", mappedRequiredCount, requiredCount);
	ImGui::SameLine();
	ImGui::ProgressBar(mappedRatio, ImVec2(200, 0));

	if (ImGui::Button("Auto Map"))
	{
		AutoMap();
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		m_RiggedData.Reset();
	}
	ImGui::SameLine();
	if (ImGui::Button("ReBuild"))
		m_RiggedData.RebuildChainsFromMap();

	if (ImGui::BeginTable("HumanoidLayoutTable", 1,
		ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::Text("Humanoid Slots");
		ImGui::Separator();
		ImVec2 avail = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("HumanoidSlotChild", ImVec2(0, avail.y), true))
		{
			if (ImGui::CollapsingHeader("Required", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (_int slotIndex = 0; slotIndex < (_int)ENUM(HumanoidBone::Count); ++slotIndex)
				{
					HumanoidBone slot = (HumanoidBone)slotIndex;
					if (!IsRequiredSlot(slot))
						continue;

					ImGui::PushID(slotIndex);

					ImGui::TextUnformatted(SlotName(slot).c_str());
					ImGui::SameLine(170);
					ImGui::SetNextItemWidth(-1.0f);

					int& mappedBoneIndex = m_RiggedData.map.indexByRole[slotIndex];

					ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, 260.0f));
					BoneCombo_Filtered("##BoneCombo", m_Bones, mappedBoneIndex, m_BoneFilter);

					ImGui::PopID();
				}
			}

			ImGui::EndChild();   
		}

		ImGui::EndTable();      
	}

	

	ImGui::End();                 
}

void CAISkeleton::Render_DynamicChain()
{
	ImGui::SetNextWindowSize(ImVec2(820, 520), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(420, 360), ImVec2(10000, 10000));
	ImGui::Begin("Dynamic Bone Tab", &m_isDynamicTabOpened);
	
	if (m_pDynamic == nullptr) {
		if (ImGui::Button("Create")) {
			m_pDynamic = CAIDynamicBone::Create(this);
		}
	}
	else {
		m_pDynamic->Render_GUI();
	}

	ImGui::End();
}

 _bool CAISkeleton::BoneCombo_Filtered(
	const char* comboId,
	const vector<CBone*>& boneList,
	int& inOutBoneIndex,
	const string& filterText)
{
	const char* previewText =
		(inOutBoneIndex >= 0 && inOutBoneIndex < (_int)boneList.size())
		? boneList[inOutBoneIndex]->Get_Name().c_str()
		: "<None>";

	bool changed = false;

	if (ImGui::BeginCombo(comboId, previewText))
	{
		// None
		if (ImGui::Selectable("<None>", inOutBoneIndex < 0))
		{
			inOutBoneIndex = -1;
			changed = true;
		}

		for (_int boneIndex = 0; boneIndex < (_int)boneList.size(); ++boneIndex)
		{
			const string& boneName = boneList[boneIndex]->Get_Name();

			if (!filterText.empty())
			{
				if (boneName.find(filterText) == string::npos)
					continue;
			}

			bool selected = (inOutBoneIndex == boneIndex);
			if (ImGui::Selectable(boneName.c_str(), selected))
			{
				inOutBoneIndex = boneIndex;
				changed = true;
			}
		}

		ImGui::EndCombo();
	}

	return changed;
}

 void CAISkeleton::AutoMap()
 {
	 // 이미 매핑된 슬롯은 건드리지 않음
	 for (const SlotNameRule& rule : s_AutoMapRules)
	 {
		 int slotIndex = (int)rule.slot;

		 if (m_RiggedData.map.indexByRole[slotIndex] >= 0)
			 continue;

		 // 본 전체 순회
		 for (_int boneIndex = 0; boneIndex < (_int)m_Bones.size(); ++boneIndex)
		 {
			 const string boneNameLower =
				 Helper::ToLower(m_Bones[boneIndex]->Get_Name());

			 // 키워드 검사
			 for (int k = 0; rule.keywords[k]; ++k)
			 {
				 if (boneNameLower.find(rule.keywords[k]) != string::npos)
				 {
					 m_RiggedData.map.indexByRole[slotIndex] = boneIndex;
					 goto NextSlot;
				 }
			 }
		 }

	 NextSlot:
		 continue;
	 }

	 m_RiggedData.RebuildChainsFromMap();
 }

HRESULT CAISkeleton::Ready_Bones(const aiNode* _pAINode, _int _iParentIndex)
{
	if (!m_BoneMap.count(string(_pAINode->mName.C_Str()))) {

		CAIBone* pBone = CAIBone::Create(_pAINode, _iParentIndex);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	_int iPIndex = m_Bones.size() - 1;

	for (_uint i = 0; i < _pAINode->mNumChildren; ++i)
	{
		Ready_Bones(_pAINode->mChildren[i], iPIndex);
	}

	m_BoneMap.emplace(_pAINode->mName.C_Str(), iPIndex);
	_float4x4 IdentityMat;
	XMStoreFloat4x4(&IdentityMat, XMMatrixIdentity());
	m_OffsetMatrices.resize(m_Bones.size(), IdentityMat);
	HasOffset.resize(m_Bones.size(), false);

	return S_OK;
}

const string CAISkeleton::SlotName(HumanoidBone boneType)
{
	switch (boneType)
	{
	case Engine::HumanoidBone::Pelvis:     return "Pelvis";
	case Engine::HumanoidBone::Spine1:     return "Spine1";
	case Engine::HumanoidBone::Spine2:     return "Spine2";
	case Engine::HumanoidBone::Chest:      return "Chest";
	case Engine::HumanoidBone::Neck:       return "Neck";
	case Engine::HumanoidBone::Head:       return "Head";
	case Engine::HumanoidBone::ClavicleL:  return "ClavicleL";
	case Engine::HumanoidBone::UpperArmL:  return "UpperArmL";
	case Engine::HumanoidBone::LowerArmL:  return "LowerArmL";
	case Engine::HumanoidBone::HandL:      return "HandL";
	case Engine::HumanoidBone::ClavicleR:  return "ClavicleR";
	case Engine::HumanoidBone::UpperArmR:  return "UpperArmR";
	case Engine::HumanoidBone::LowerArmR:  return "LowerArmR";
	case Engine::HumanoidBone::HandR:      return "HandR";
	case Engine::HumanoidBone::ThighL:     return "ThighL";
	case Engine::HumanoidBone::CalfL:      return "CalfL";
	case Engine::HumanoidBone::FootL:      return "FootL";
	case Engine::HumanoidBone::ToeL:       return "ToeL";
	case Engine::HumanoidBone::ThighR:     return "ThighR";
	case Engine::HumanoidBone::CalfR:      return "CalfR";
	case Engine::HumanoidBone::FootR:      return "FootR";
	case Engine::HumanoidBone::ToeR:       return "ToeR";
	default:                     return "Unknown";
	}
}

_bool CAISkeleton::IsRequiredSlot(HumanoidBone slot)
{
	switch (slot)
	{ /*필요 슬롯*/
	case Engine::HumanoidBone::Pelvis:
	case Engine::HumanoidBone::Spine1:
	case Engine::HumanoidBone::Neck:
	case Engine::HumanoidBone::Head:
	case Engine::HumanoidBone::UpperArmL:
	case Engine::HumanoidBone::LowerArmL:
	case Engine::HumanoidBone::HandL:
	case Engine::HumanoidBone::UpperArmR:
	case Engine::HumanoidBone::LowerArmR:
	case Engine::HumanoidBone::HandR:
	case Engine::HumanoidBone::ThighL:
	case Engine::HumanoidBone::CalfL:
	case Engine::HumanoidBone::FootL:
	case Engine::HumanoidBone::ToeL:
	case Engine::HumanoidBone::ThighR:
	case Engine::HumanoidBone::CalfR:
	case Engine::HumanoidBone::FootR:
	case Engine::HumanoidBone::ToeR:
		return true;
	default:
		return false;
	}
}

_bool CAISkeleton::CheckAncester(_int ancestorIndex, _int childIndex)
{
	if (ancestorIndex < 0 || childIndex < 0)
		return false;

	_int Chain = childIndex;
	while (Chain >= 0) {
		if (Chain == ancestorIndex)
			return true;
		Chain = m_Bones[Chain]->Get_ParentIndex();
	}
	return false;
}

void CAISkeleton::Save_File(ofstream& ofs, _fmatrix PreTransform)
{
	SKELETON_FILE_HEADER skeleton = {};
	skeleton.BoneCount = m_Bones.size();
	ofs.write(reinterpret_cast<const char*>(&skeleton), sizeof(SKELETON_FILE_HEADER));

	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		static_cast<CAIBone*>(m_Bones[i])->Save_File(ofs, PreTransform);
	}

	for (size_t i = 0; i < m_OffsetMatrices.size(); i++)
	{
		_float4x4 SaveFloat4x4 = {};
		_matrix SaveMatrix = XMLoadFloat4x4(&m_OffsetMatrices[i]);
		XMStoreFloat4x4(&SaveFloat4x4, SaveMatrix);
		ofs.write(reinterpret_cast<const char*>(&SaveFloat4x4), sizeof(_float4x4));
	}

	_bool hasRiggedData = m_RiggedData.IsRigComplete();
	ofs.write(reinterpret_cast<const char*>(&hasRiggedData), sizeof(_bool));
	if (hasRiggedData) {
		ofs.write(reinterpret_cast<const char*>(&m_RiggedData), sizeof(HumanoidRigData));
	}

	_bool hasDynamic = (m_pDynamic != nullptr);
	ofs.write(reinterpret_cast<const char*>(&hasDynamic), sizeof(_bool));
	if (m_pDynamic)
		m_pDynamic->Save_File(ofs);
}

void CAISkeleton::Rake_BoneInfo(BONE_DATA_HEADER* pHeader)
{
	pHeader->BoneInfos.reserve(m_Bones.size());

	for (size_t i = 0; i < m_OffsetMatrices.size(); i++)
	{
		BONE_INFO boneinfo = {};

		boneinfo.Index = i;
		boneinfo.BoneOffsetMatrix = m_Bones[i]->Get_TransformationMatrix();
		boneinfo.TagBone = m_Bones[i]->Get_Name();

		pHeader->BoneInfos.push_back(boneinfo);
	}
}

CAISkeleton* CAISkeleton::Create(const aiNode* _pAINode)
{
	CAISkeleton* pInstance = new CAISkeleton();

	if (FAILED(pInstance->Initialize(_pAINode))) {
		MSG_BOX("Create Failed : Engine | CAISkeleton");
		return nullptr;
	}

	return pInstance;
}
void CAISkeleton::Free()
{
	__super::Free();
}
