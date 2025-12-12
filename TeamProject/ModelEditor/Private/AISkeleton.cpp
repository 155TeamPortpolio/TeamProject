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


_bool CAISkeleton::Find_BoneIndex(const _char* pName, _uint* _iGetIndex)
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(),
		[&](CAIBone* pBone) { return pBone->Compare_Name(pName); });

	if (iter == m_Bones.end())
		return false;

	if (nullptr != _iGetIndex)
		(*_iGetIndex) = static_cast<_int>(distance(m_Bones.begin(), iter));

	return true;
}

const CAIBone* CAISkeleton::Find_Bone(const _char* pName) const
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(),
		[&](CAIBone* pBone) { return pBone->Compare_Name(pName); });

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter);
}

HRESULT CAISkeleton::Ready_Bones(const aiNode* _pAINode, _int _iParentIndex)
{
	CAIBone* pBone = CAIBone::Create(_pAINode, _iParentIndex);

	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iPIndex = (_int)m_Bones.size() - 1;

	for (_uint i = 0; i < _pAINode->mNumChildren; i++)
	{
		Ready_Bones(_pAINode->mChildren[i], iPIndex);
	}

	return S_OK;
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
