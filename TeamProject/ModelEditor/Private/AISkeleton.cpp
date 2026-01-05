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
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		string name = "(" + to_string(i) + ")" + m_Bones[i]->Get_Name();
		ImGui::Text(name.c_str());
		if (ImGui::IsItemHovered())
		{
			_int parentIndex = m_Bones[i]->Get_ParentIndex();

			if (parentIndex != -1) {
				CBone* pParent = m_Bones[parentIndex];
				ImGui::SetTooltip("Parent: %s", pParent->Get_Name().c_str());
			}
			else
				ImGui::SetTooltip("Parent: <None>");
		}
	}

	for (size_t i = 0; i < ErroredOffset.size(); i++)
	{
		ImGui::Text("Offset Overrided");
	}
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
