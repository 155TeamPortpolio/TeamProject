#include "AIModelData.h"
#include "AISkeleton.h"
#include "AIMesh.h"
#include "Helper_Func.h"


CAIModelData::CAIModelData()
{
}

HRESULT CAIModelData::Initialize(MESH_TYPE _eType, const aiScene* pAIScene)
{
    m_pSkeleton = CAISkeleton::Create(pAIScene->mRootNode);
    _uint meshNum = pAIScene->mNumMeshes;

    unordered_set<string> loadedMeshNames;
    loadedMeshNames.reserve(meshNum);

    for (size_t i = 0; i < meshNum; i++)
    {
        aiMesh* srcMesh = pAIScene->mMeshes[i];
        string meshName = Helper::ToLower(srcMesh->mName.C_Str());

        if (!meshName.empty() && loadedMeshNames.find(meshName) != loadedMeshNames.end())
            continue;

        if (!meshName.empty())
            loadedMeshNames.insert(meshName);

        CAIMesh* pMesh = CAIMesh::Create(_eType, srcMesh, static_cast<CAISkeleton*>(m_pSkeleton));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);

        string keyLower = Helper::ToLower(pMesh->Get_Key());

        if (keyLower.find("proxy") != string::npos)
            m_ProxyMarked.push_back(m_Meshes.size() - 1);
        if (keyLower.find("lod0") != string::npos)
            m_LOD0Marked.push_back(m_Meshes.size() - 1);
        if (keyLower.find("lod1") != string::npos)
            m_LOD1Marked.push_back(m_Meshes.size() - 1);
        if (keyLower.find("lod2") != string::npos)
            m_LOD2Marked.push_back(m_Meshes.size() - 1);
        if (keyLower.find("lod3") != string::npos)
            m_LOD3Marked.push_back(m_Meshes.size() - 1);
        if (keyLower.find("eff") != string::npos)
            m_EffMarked.push_back(m_Meshes.size() - 1);
    }

    for (auto mesh : m_Meshes)
    {
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