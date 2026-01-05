#include "pch.h"
#include "AISkeleton.h"
#include "AIBone.h"

CAISkeleton::CAISkeleton()
{
}

HRESULT CAISkeleton::Initialize(const aiNode* _pAINode)
{
	if (FAILED(Ready_Bones(_pAINode)))
		return E_FAIL;

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
		if(newOne != oldOne)
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
}
void CAISkeleton::Render_Humanoid()
{
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

	ImGui::End();
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
	{ /*ÇÊ¿ä ½½·Ô*/
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
	case Engine::HumanoidBone::ThighR:
	case Engine::HumanoidBone::CalfR:
	case Engine::HumanoidBone::FootR:
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
	while (Chain >=0) {
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
