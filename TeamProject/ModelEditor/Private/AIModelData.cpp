#include "AIModelData.h"
#include "AISkeleton.h"
#include "AIMesh.h"
#include "Helper_Func.h"
#include "EditorSystem.h"

HRESULT CAIModelData::Initialize(MESH_TYPE _eType, const aiScene* pAIScene)
{
    m_pSkeleton = CAISkeleton::Create(pAIScene->mRootNode);
    _uint meshNum = pAIScene->mNumMeshes;

    unordered_set<string> loadedMeshNames;
    loadedMeshNames.reserve(meshNum);

    constexpr float epsilon = 30;

    for (size_t meshIndex = 0; meshIndex < meshNum; ++meshIndex)
    {
        aiMesh* srcMesh = pAIScene->mMeshes[meshIndex];
        string meshName = Helper::ToLower(srcMesh->mName.C_Str());

        if (!CEditorSystem::GetInstance()->CheckNamingRule(meshName))
            continue;

        CAIMesh* pSourceMesh = CAIMesh::Create(_eType, srcMesh, static_cast<CAISkeleton*>(m_pSkeleton));
        if (nullptr == pSourceMesh)
            return E_FAIL;

        const vector<_uint>& sourceIndices = pSourceMesh->Get_Indices();
        _uint sourceVertexCount = pSourceMesh->Get_VertexCount();

        if (sourceIndices.empty() || (sourceIndices.size() % 3 != 0) || sourceVertexCount == 0 || _eType == MESH_TYPE::ANIM)
        {
            m_Meshes.push_back(pSourceMesh);
            continue;
        }

        const _uint faceCount = (_uint)(sourceIndices.size() / 3);

        vector<vector<_uint>> islands;

        if (_eType == MESH_TYPE::NONANIM)
        {
            const vector<VTXMESH>& sourceVertices = pSourceMesh->Get_StaticVertices();
            islands = Find_Island_ByPosition(sourceVertices, sourceIndices, epsilon);
        }
        else
        {
            const vector<VTXSKINMESH>& sourceVertices = pSourceMesh->Get_SkinnedVertices();
            islands = Find_Island_ByPosition(sourceVertices, sourceIndices, epsilon);
        }

       if (islands.empty() || islands.size() <= 1 || islands.size() >= (size_t)(faceCount * 9 / 10))
        {
            m_Meshes.push_back(pSourceMesh);
            continue;
        }

        for (size_t islandIndex = 0; islandIndex < islands.size(); ++islandIndex)
        {
            string newKey = "Isl_" + string(srcMesh->mName.C_Str()) + to_string(islandIndex);

            CAIMesh* islandMesh = CAIMesh::Create();
            if (nullptr == islandMesh)
            {
                Safe_Release(pSourceMesh);
                return E_FAIL;
            }

            if (_eType == MESH_TYPE::NONANIM)
            {
                const vector<VTXMESH>& sourceVertices = pSourceMesh->Get_StaticVertices();

                vector<VTXMESH> cookedVertices;
                vector<_uint> cookedIndices;
                Build_IslandMesh(sourceVertexCount, sourceVertices, sourceIndices,
                    islands[islandIndex], cookedVertices, cookedIndices);

                if (FAILED(islandMesh->Initialize_FromCooked_NonAnim(
                    newKey,
                    pSourceMesh->Get_MaterialIndex(),
                    static_cast<CAISkeleton*>(m_pSkeleton),
                    cookedVertices,
                    cookedIndices,
                    pSourceMesh->Get_MeshOffset(),
                    pSourceMesh->Get_BoneIndices())))
                {
                    Safe_Release(islandMesh);
                    Safe_Release(pSourceMesh);
                    return E_FAIL;
                }
            }
            else
            {
                const vector<VTXSKINMESH>& sourceVertices = pSourceMesh->Get_SkinnedVertices();

                vector<VTXSKINMESH> cookedVertices;
                vector<_uint> cookedIndices;
                Build_IslandMesh(sourceVertexCount, sourceVertices, sourceIndices,
                    islands[islandIndex], cookedVertices, cookedIndices);

                if (FAILED(islandMesh->Initialize_FromCooked_Anim(
                    newKey,
                    pSourceMesh->Get_MaterialIndex(),
                    static_cast<CAISkeleton*>(m_pSkeleton),
                    cookedVertices,
                    cookedIndices,
                    pSourceMesh->Get_MeshOffset(),
                    pSourceMesh->Get_BoneIndices())))
                {
                    Safe_Release(islandMesh);
                    Safe_Release(pSourceMesh);
                    return E_FAIL;
                }
            }

            m_Meshes.push_back(islandMesh);

        }

        Safe_Release(pSourceMesh);
    }
    for (size_t i = 0; i < m_Meshes.size(); i++)
    {
        string meshName = Helper::ToLower(m_Meshes[i]->Get_Key());
        if (meshName.find("lod1") != string::npos) 
            m_LOD1Marked.push_back(i);
        else if (meshName.find("lod2") != string::npos) 
            m_LOD2Marked.push_back(i);
        else if (meshName.find("lod3") != string::npos) 
            m_LOD3Marked.push_back(i);
        else 
            m_Normal.push_back(i);
    }

    return S_OK;
}

vector<_uint> CAIModelData::Get_MeshIndex_WithOutProxy()
{
	return m_NotProxy;
}

vector<vector<_uint>> CAIModelData::Find_Island(_uint numVertices, const vector<_uint>& indices)
{
	if (indices.size() % 3 != 0)
    {
		MSG_BOX("Not 3 vertex for Face");
		return{};
	};
	_uint faceCount = static_cast<_uint>(indices.size() / 3);
	vector<vector<_uint>> vertexToFace;
	vertexToFace.resize(numVertices);

	for (_uint face = 0; face < faceCount; ++face)
    {
		_uint index0 = indices[face * 3 + 0];
		_uint index1 = indices[face * 3 + 1];
		_uint index2 = indices[face * 3 + 2];
		if (index0 < numVertices) vertexToFace[index0].push_back(face);
		if (index1 < numVertices) vertexToFace[index1].push_back(face);
		if (index2 < numVertices) vertexToFace[index2].push_back(face);
	}

	vector<_uint> visited(faceCount, 0);
	vector<_uint> stack;
	vector<vector<_uint>> islands; 
	islands.reserve(8);

	for (_uint face = 0; face < faceCount; ++face)
    {
		if (visited[face]) continue;
		visited[face] = 1;

		islands.emplace_back();
		auto& IslandFace = islands.back();
		stack.clear();
		stack.push_back(face);

		while (!stack.empty())
        {
			_uint faceNum = stack.back(); stack.pop_back();
			IslandFace.push_back(faceNum);

			_uint index0 = indices[faceNum * 3 + 0];
			_uint index1 = indices[faceNum * 3 + 1];
			_uint index2 = indices[faceNum * 3 + 2];

			auto pushNeighbors = [&](_uint num) 
                {
				if (num >= numVertices) return;
				for (_uint neighbor : vertexToFace[num])
                {
					if (!visited[neighbor]) 
                    {
						visited[neighbor] = 1;
						stack.push_back(neighbor);
					}
				}
				};

			pushNeighbors(index0);
			pushNeighbors(index1);
			pushNeighbors(index2);
		}
	}

	return islands;
}

CModelData* CAIModelData::Create(MESH_TYPE _eType, const aiScene* pAIScene)
{
	CAIModelData* pInstance = new CAIModelData();
	if (FAILED(pInstance->Initialize(_eType, pAIScene)))
    {
		MSG_BOX("Create Failed : Engine | CAIModelData");
		return nullptr;
	}
	return pInstance;
}

void CAIModelData::Save_File(ofstream& ofs, _fmatrix PreTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); i++)
		static_cast<CAIMesh*>(m_Meshes[i])->Save_File(ofs, PreTransform);
	static_cast<CAISkeleton*>(m_pSkeleton)->Save_File(ofs, PreTransform);
}