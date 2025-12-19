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