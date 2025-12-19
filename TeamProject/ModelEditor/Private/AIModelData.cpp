#include "AIModelData.h"
#include "AISkeleton.h"
#include "AIMesh.h"

CAIModelData::CAIModelData()
{
}

HRESULT CAIModelData::Initialize(MESH_TYPE _eType, const aiScene* pAIScene)
{
	m_pSkeleton = CAISkeleton::Create(pAIScene->mRootNode);
	_uint meshNum = pAIScene->mNumMeshes;
	
	for (size_t i = 0; i < meshNum; i++)
	{
		CAIMesh* pMesh = CAIMesh::Create(_eType, pAIScene->mMeshes[i], static_cast<CAISkeleton*>(m_pSkeleton));
		if (nullptr == pMesh)
			return E_FAIL;
		m_Meshes.push_back(pMesh);
		if (pMesh->Get_Key().find("Proxy") != string::npos) 
			m_ProxyMarked.push_back(m_Meshes.size() - 1);
		if (pMesh->Get_Key().find("Lod1") != string::npos) 
			m_ProxyMarked.push_back(m_Meshes.size() - 1);
		if (pMesh->Get_Key().find("Lod2") != string::npos) 
			m_ProxyMarked.push_back(m_Meshes.size() - 1);
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

void CAIModelData::Save_File(ofstream& ofs, _fmatrix PreTransform)
{

	for (size_t i = 0; i < m_Meshes.size(); i++)
	{
		static_cast<CAIMesh*>(m_Meshes[i])->Save_File(ofs, PreTransform);
	}
	static_cast<CAISkeleton*>(m_pSkeleton)->Save_File(ofs, PreTransform);
}

void CAIModelData::Free()
{
	__super::Free();
}