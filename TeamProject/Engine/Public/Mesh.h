#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)
class ENGINE_DLL CMesh :
	public CVIBuffer
{
	struct IslandRange /*같은 인덱스 버퍼를 공유하는 아이들의 모음*/
	{
		_uint startIndex = {};
		_uint indexCount = {};
		MINMAX_BOX localBox{};
	};

	struct UnionFind {
		
	};
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

public:
	 HRESULT Render_Island(ID3D11DeviceContext* pContext, _uint islandIndex);

private:
	void Build_Island();

public:
	void Create_BoneMinMax(class CSkeleton* pSkeleton);
	void ExpandBox(MINMAX_BOX& b, const _float3& p);
	virtual void Render_GUI();

protected:
	vector<_uint> m_indices;										
	vector<VTXMESH> m_StaticVertex;
	vector<VTXSKINMESH> m_Skined;					

	_uint m_OffsetCount = {  };
	unordered_map<_uint,_float4x4> m_MeshOffset; /*어떤 본과 관련된 오프셋인가*/

	_uint m_MaterialIndex = {};					//어떤 머티리얼을 사용하는가??
	_float3 m_vMeshMinLocal = { FLT_MAX ,FLT_MAX ,FLT_MAX };
	_float3 m_vMeshMaxLocal = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };
	
	vector<IslandRange> m_Islands;

public:
	static CMesh* Create(ID3D11Device* pDevice,ifstream& ifs, MESH_TYPE eType);
	virtual void Free() override;
};
NS_END
