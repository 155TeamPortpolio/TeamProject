#pragma once
#include "ModelEditor_Defines.h"
#include "Assimps.h"
#include "Mesh.h"

NS_BEGIN(ModelEdit)

class CAIMesh final :
	public CMesh
{
private:
	CAIMesh();
	CAIMesh(const string& ModelKey);
	virtual ~CAIMesh() DEFAULT;

public:
	void Get_MeshInfo(MESH_INFO_HEADER* OutMeshInfoHeader) const {
		strcpy_s(OutMeshInfoHeader->MeshName, sizeof(MAX_PATH), m_VIKey.c_str());
		OutMeshInfoHeader->VerticesCount = m_iVerticesCount;
		OutMeshInfoHeader->IndicesCount = m_iIndicesCount;
		OutMeshInfoHeader->MaterialIndex = m_MaterialIndex;
		OutMeshInfoHeader->BoneCount = (_uint)m_BoneIndices.size();
	}

public:
	virtual HRESULT Initialize(const aiMesh* _pAIMesh, MESH_TYPE _eMeshType, class CAISkeleton* _pSkeleton);

public:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* _pAIMesh);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* _pAIMesh, class CAISkeleton* _pSkeleton);
	void Save_File(ofstream& ofs);

private:
	//ÀúÀå¿ë
	vector<VTXSKINMESH> m_SkinMeshes;
	vector<VTXMESH>		m_Meshes;
	vector<_uint>		m_Indices;
	vector<_uint>		m_BoneIndices;
	CSkeleton* m_pSkeleton = { nullptr };

public:
	static CAIMesh* Create(MESH_TYPE _eType, const aiMesh* _pAIMesh, class CAISkeleton* _pSkeleton);
	virtual void Free() override;
};

NS_END