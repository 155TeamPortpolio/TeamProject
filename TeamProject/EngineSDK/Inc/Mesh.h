#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)
class ENGINE_DLL CMesh :
	public CVIBuffer
{
protected:
	CMesh();
	CMesh(const string& ModelKey);
	virtual ~CMesh();

public:
	virtual HRESULT Initialize_From_File(ID3D11Device* pDevice, ifstream& ifs, MESH_TYPE eType);

private:
	virtual HRESULT Create_AnimateVertex(ID3D11Device* pDevice, ifstream& ifs) ;
	virtual HRESULT Create_StaticVertex(ID3D11Device* pDevice, ifstream& ifs) ;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;
	virtual HRESULT Finalize_AnimateVertexVB(ID3D11Device* pDevice);

public:
	_uint Get_MaterialIndex() { return m_MaterialIndex; }
	_float3 Get_MinVertexLocal() { return m_vMeshMinLocal; }
	_float3 Get_MaxVertexLocal() { return m_vMeshMaxLocal; }

	const vector<_uint>& Get_Indices() { return m_indices; }
	const vector<VTXMESH>& Get_StaticVertices() { return m_StaticVertex; }
	const vector<VTXSKINMESH>& Get_SkinnedVertices() { return m_Skined; }


	virtual _uint Get_StaticVerticesCount() { return m_iVerticesCount; }
	virtual _uint Get_SkinnedVerticesCount() { return m_iVerticesCount; }
	_float4x4 Get_MeshOffset(_uint boneIndex);
	const unordered_map<_uint, _float4x4>& Get_MeshOffsetBones() { return m_MeshOffset; };

public:
	void Create_BoneMinMax(class CSkeleton* pSkeleton);
	void ExpandBox(MINMAX_BOX& b, const _float3& p);
	virtual void Render_GUI();

public:
	HRESULT BakeSkinRemapAndRebuildVB(ID3D11Device* pDevice, _uint skeletonBoneCount);
	const vector<uint16_t>& Get_UsedBones() const { return m_UsedBones; }
	
protected:
	void BuildUsedBonesAndRemap(_uint skeletonBoneCount);
	void RemapVertexBlendIndices();
	bool HasWeight(const XMFLOAT4& weight, int lane);
	uint32_t GetBlendIndexLane(const XMUINT4& blendIndex, int lane);
protected:
	vector<_uint> m_indices;										
	vector<VTXMESH> m_StaticVertex;
	vector<VTXSKINMESH> m_Skined;					

	_uint m_OffsetCount = {  };
	unordered_map<_uint,_float4x4> m_MeshOffset; /*어떤 본과 관련된 오프셋인가*/

	_uint m_MaterialIndex = {};					//어떤 머티리얼을 사용하는가??
	_float3 m_vMeshMinLocal = { FLT_MAX ,FLT_MAX ,FLT_MAX };
	_float3 m_vMeshMaxLocal = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };
	
protected:
	vector<uint16_t> m_UsedBones;
	vector<uint16_t> m_GlobalToLocal;
public:
	static CMesh* Create(ID3D11Device* pDevice,ifstream& ifs, MESH_TYPE eType);
	virtual void Free() override;
};
NS_END
