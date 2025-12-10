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

public:
	_uint Get_MaterialIndex() { return m_MaterialIndex; }
	_float3 Get_MinVertexLocal() { return m_vMeshMinLocal; }
	_float3 Get_MaxVertexLocal() { return m_vMeshMaxLocal; }

public:
	void Create_BoneMinMax(class CSkeleton* pSkeleton);
	virtual void Render_GUI();

protected:
	vector<_uint> m_indices;						//임시 인덱스 (함수로 따로 빼려고 하다보니 생김)
	vector<VTXSKINMESH> m_Skined;						//임시 버텍스 (본 기준의 민맥스 따로 만드려다 보니.)
	_uint m_MaterialIndex = {};					//어떤 머티리얼을 사용하는가??

	_float3 m_vMeshMinLocal = { FLT_MAX ,FLT_MAX ,FLT_MAX };
	_float3 m_vMeshMaxLocal = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };
	
public:
	static CMesh* Create(ID3D11Device* pDevice,ifstream& ifs, MESH_TYPE eType);
	virtual void Free() override;
};
NS_END
