#include "AIModelData.h"
#include "AISkeleton.h"
#include "AIMesh.h"

CAIModelData::CAIModelData()
{
}

HRESULT CAIModelData::Initialize(MESH_TYPE _eType, const aiScene* pAIScene)
{
	m_pSkeleton = CAISkeleton::Create(pAIScene->mRootNode);

	for (int i = 0; i < pAIScene->mNumMeshes; ++i) {
		CAIMesh* newAIMesh = CAIMesh::Create(_eType, pAIScene->mMeshes[i], static_cast<CAISkeleton*>(m_pSkeleton));
		if (newAIMesh)
			m_Meshes.push_back(newAIMesh);
		else
		{
			for (auto& mesh : m_Meshes)
				Safe_Release(mesh);
			return E_FAIL;
		}
	}

	return S_OK;
}

CModelData* CAIModelData::Create(MESH_TYPE _eType, const aiScene* pAIScene)
{
	CAIModelData* pInstance = new CAIModelData();

	if (FAILED(pInstance->Initialize(_eType, pAIScene))) {
		MSG_BOX("Create Failed : Engine | CAIModelData");
		return nullptr;
	}

	return pInstance;
}

void CAIModelData::Free()
{
	__super::Free();
}