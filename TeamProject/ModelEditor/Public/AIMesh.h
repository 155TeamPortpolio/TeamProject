#pragma once
#include "ModelEditor_Defines.h"
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

	virtual HRESULT Initialize_FromCooked_NonAnim(
		const string& newKey,
		_uint materialIndex,
		class CAISkeleton* skeleton,
		const vector<VTXMESH>& cookedVertices,
		const vector<_uint>& cookedIndices,
		const unordered_map<_uint, _float4x4>& meshOffsetMap,
		const vector<_uint>& boneIndexList
	);

	virtual HRESULT Initialize_FromCooked_Anim(
		const string& newKey,
		_uint materialIndex,
		class CAISkeleton* skeleton,
		const vector<VTXSKINMESH>& cookedVertices,
		const vector<_uint>& cookedIndices,
		const unordered_map<_uint, _float4x4>& meshOffsetMap,
		const vector<_uint>& boneIndexList
	);

public:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* _pAIMesh);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* _pAIMesh, class CAISkeleton* _pSkeleton);
	void Save_File(ofstream& ofs, _fmatrix PreTransform);

public:
	const unordered_map<_uint, _float4x4>& Get_MeshOffset() { return m_MeshOffset; };
	const vector<_uint>& Get_BoneIndices() { return m_BoneIndices; };
private:
	//�����
	vector<VTXSKINMESH> m_SkinMeshes;
	vector<VTXMESH>		m_Meshes;
	vector<_uint>		m_Indices;
	vector<_uint>		m_BoneIndices;

	CSkeleton* m_pSkeleton = { nullptr };
	_int BoneIndex = {-1};
	_bool isRigid = { false };
	_uint VertexCount = {};
public:
	static CAIMesh* Create(MESH_TYPE _eType, const aiMesh* _pAIMesh, class CAISkeleton* _pSkeleton);
	static CAIMesh* Create();
	virtual void Free() override;
};

NS_END