#include "AIModelData.h"
#include "AISkeleton.h"
#include "AIMesh.h"
#include "Helper_Func.h"
#include "EditorSystem.h"

CAIModelData::CAIModelData()
{
}
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

        // ï¿½ï¿½ï¿½ï¿½ ï¿½Þ½ï¿½ ï¿½ï¿½ï¿½ï¿½
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
        if (meshName.find("lod1") != string::npos) {
            m_LOD1Marked.push_back(i);
        }
        else if (meshName.find("lod2") != string::npos) {
            m_LOD2Marked.push_back(i);
        }
        else if (meshName.find("lod3") != string::npos) {
            m_LOD3Marked.push_back(i);
        }
        else {
            m_Normal.push_back(i);
        }

    }

    return S_OK;
}

vector<_uint> CAIModelData::Get_MeshIndex_WithOutProxy()
{
	return m_NotProxy;
}

void CAIModelData::Render_GUI()
{
    if (!m_Meshes.empty()) {
        if(ImGui::Button("MeshOpen"))
        isGui_MeshTabOpen = !isGui_MeshTabOpen;
    }

    if (isGui_MeshTabOpen) {
        if (ImGui::Begin("Meshes", &isGui_MeshTabOpen, ImGuiWindowFlags_NoCollapse))
        {
            for (size_t i = 0; i < m_Meshes.size(); i++)
            {
                string meshBtn = m_Meshes[i]->Get_Key() + "##Btn" + to_string(i);
                if (ImGui::Button(meshBtn.c_str())) {
                    selectedMeshIndex = i;
                    isGui_MeshTableTabOpen = true;
                }
            }
        }
        ImGui::End();
    }

    if (isGui_MeshTableTabOpen) {
        if (ImGui::Begin("MeshTab", &isGui_MeshTableTabOpen, ImGuiWindowFlags_NoCollapse))
        {
            m_Meshes[selectedMeshIndex]->Render_GUI();
        }
        ImGui::End();
    }
}



vector<vector<_uint>> CAIModelData::Find_Island(_uint numVertices, const vector<_uint>& indices)
{
	if (indices.size() % 3 != 0)  {
		MSG_BOX("Not 3 vertex for Face");
		return{};
	};
	_uint faceCount = static_cast<_uint>(indices.size() / 3);
	vector<vector<_uint>> vertexToFace;
	vertexToFace.resize(numVertices);

	// vertex -> faces
	for (_uint face = 0; face < faceCount; ++face) {
		/*0ï¿½ï¿½Â° ï¿½ï°¢ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½*/
		_uint index0 = indices[face * 3 + 0];
		_uint index1 = indices[face * 3 + 1];
		_uint index2 = indices[face * 3 + 2];
		/*ï¿½ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½ ï¿½ï¿½È£ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì½ï¿½ï¿½ï¿½ ï¿½ï¿½È£ï¿½ï¿½ ï¿½Ö¾ï¿½ï¿½ï¿½*/
		if (index0 < numVertices) vertexToFace[index0].push_back(face);
		if (index1 < numVertices) vertexToFace[index1].push_back(face);
		if (index2 < numVertices) vertexToFace[index2].push_back(face);
	}
	/*ÀÌ·¯¸é Á¤Á¡ x°¡ Æ÷ÇÔµÈ ¸ðµç ÆäÀÌ½º¸¦ Ã£¾Æ¿Ã ¼ö ÀÖ´Ù. -> Á¤Á¡ 0¹ø¿¡ 1,2 / Á¤Á¡ 1¹ø¿¡ 2/3 ÀÌ·±½ÄÀ¸·Î*/

	vector<_uint> visited(faceCount, 0); /*ÆäÀÌ½ºµéÀÌ ÀÌ¹Ì ±â·ÏÀÌ µÇ¾ú´ÂÁö Ã¼Å©ÇÏ´Â °Í. (Æ¯Á¤ ¼¶¿¡ Æ÷ÇÔµÇ¸é Áßº¹ Æ÷ÇÔ ¾ÈÇÏ°Ô)*/
	vector<_uint> stack; /*´ÙÀ½¿¡ Ã³¸®ÇØº¼ ¸ñ·Ï*/
	vector<vector<_uint>> islands; /*¼¶¿¡ Æ÷ÇÔµÈ ÆäÀÌ½º ¹øÈ£µé*/
	islands.reserve(8);

	for (_uint face = 0; face < faceCount; ++face) {
		if (visited[face]) continue; /*1ÀÌ»óÀÌ¸é Æ®·ç ¶³¾îÁö´Ï±î ÀÌ¹Ì Æ÷ÇÔµÈ ÆäÀÌ½ºÀÓ*/
		/*¾Æ´Ï¸é ¸¶Å·ÇØµÎ°í*/
		visited[face] = 1;

		islands.emplace_back(); /*ÇÏ³ª ¸¸µé¾îµÎ°í µÚ¿¡°Å °¡Á®¿À±â*/
		auto& IslandFace = islands.back();
		stack.clear();
		stack.push_back(face); //ÀÏ´Ü ÆäÀÌ½º ÇÏ³ª ²¨³»±â

		while (!stack.empty()) {
			_uint faceNum = stack.back(); stack.pop_back();
			IslandFace.push_back(faceNum); //ÀÌ¹ø ÆäÀÌ½º¸¦ ÇØ´ç ¼¶¿¡ ³Ö±â

			_uint index0 = indices[faceNum * 3 + 0];
			_uint index1 = indices[faceNum * 3 + 1];
			_uint index2 = indices[faceNum * 3 + 2];

			//ÇØ´ç ÆäÀÌ½ºÀÇ Á¤Á¡À» º¸°í, ±× Á¤Á¡À» °øÀ¯ÇÏ´Â ¸éµµ ÇÔ²² ¼¶¿¡ ³Ö±â
			auto pushNeighbors = [&](_uint num) {
				if (num >= numVertices) return;
				for (_uint neighbor : vertexToFace[num]) {/*Á¤Á¡ ¹øÈ£¿¡ ¾î¶² ÆäÀÌ½º°¡ ÀÖ´ÂÁö À§¿¡ ÀúÀåÇßÀ¸´Ï*/
					if (!visited[neighbor]) {
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

void CAIModelData::Rake_SkeletonInfo(BONE_DATA_HEADER* pHeader)
{
    //dynamic_cast<CAISkeleton*>(m_pSkeleton)->Rake_BoneInfo(pHeader);
    
}

void CAIModelData::Free()
{
	__super::Free();
}