#include "Engine_Defines.h"
#include "Mesh.h"
#include "Bone.h"
#include "Skeleton.h"

CMesh::CMesh()
	: CVIBuffer("")
{
}
CMesh::CMesh(const string& ModelKey)
	:CVIBuffer(ModelKey)
{
}

CMesh::~CMesh()
{
}

HRESULT CMesh::Initialize_From_File(ID3D11Device* pDevice, ifstream& ifs, MESH_TYPE eType)
{
	MESH_INFO_HEADER infoHeader = {};

 	ifs.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
	m_VIKey = string(infoHeader.MeshName);
	m_MaterialIndex = infoHeader.MaterialIndex;
	m_iVertexBufferCount = 1;
	m_iVerticesCount = infoHeader.VerticesCount;
	m_iVertexStride = eType == MESH_TYPE::ANIM ? sizeof(VTXSKINMESH) : sizeof(VTXMESH);
	m_iIndicesCount = infoHeader.IndicesCount;
	m_iIndexStride = 4; //byte
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_ElementCount = eType == MESH_TYPE::ANIM ? VTXSKINMESH::iElementCount : VTXMESH::iElementCount;
	m_ElementKey = eType == MESH_TYPE::ANIM ? VTXSKINMESH::Key : VTXMESH::Key;
	m_ElementDesc = eType == MESH_TYPE::ANIM ? VTXSKINMESH::Elements : VTXMESH::Elements;

	HRESULT hr = eType == MESH_TYPE::ANIM ? Create_AnimateVertex(pDevice, ifs) : Create_StaticVertex(pDevice, ifs);

	if (FAILED(hr))
		return E_FAIL;
	
	m_indices.resize(infoHeader.IndicesCount);
	ifs.read(reinterpret_cast<char*>(m_indices.data()), m_iIndicesCount * m_iIndexStride);

	if (FAILED(Create_Index(pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Create_AnimateVertex(ID3D11Device* pDevice, ifstream& ifs)
{
	m_ElementCount = VTXSKINMESH::iElementCount;
	m_ElementKey = VTXSKINMESH::Key;
	m_ElementDesc = VTXSKINMESH::Elements;

	vector<VTXSKINMESH>vertices = {};
	vertices.resize(m_iVerticesCount);

	ifs.read(reinterpret_cast<char*>(vertices.data()), m_iVerticesCount * m_iVertexStride);

	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertices.data();

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	m_Skined = vertices;
	return hr;
}

HRESULT CMesh::Create_StaticVertex(ID3D11Device* pDevice, ifstream& ifs)
{
	m_ElementCount = VTXMESH::iElementCount;
	m_ElementKey = VTXMESH::Key;
	m_ElementDesc = VTXMESH::Elements;
	vector<VTXMESH>vertices = {};
	vertices.resize(m_iVerticesCount);

	ifs.read(reinterpret_cast<char*>(vertices.data()), m_iVerticesCount * m_iVertexStride);

	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertices.data();

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	m_vMeshMinLocal = { FLT_MAX,FLT_MAX ,FLT_MAX };
	m_vMeshMaxLocal = { -FLT_MAX,-FLT_MAX ,-FLT_MAX };

	for (const auto& vertex : vertices) {
		m_vMeshMinLocal.x = min(m_vMeshMinLocal.x, vertex.vPosition.x);
		m_vMeshMinLocal.y = min(m_vMeshMinLocal.y, vertex.vPosition.y);
		m_vMeshMinLocal.z = min(m_vMeshMinLocal.z, vertex.vPosition.z);

		m_vMeshMaxLocal.x = max(m_vMeshMaxLocal.x, vertex.vPosition.x);
		m_vMeshMaxLocal.y = max(m_vMeshMaxLocal.y, vertex.vPosition.y);
		m_vMeshMaxLocal.z = max(m_vMeshMaxLocal.z, vertex.vPosition.z);
	}
	if (m_vMeshMinLocal.x > m_vMeshMaxLocal.x) swap(m_vMeshMinLocal.x, m_vMeshMaxLocal.x);
	if (m_vMeshMinLocal.y > m_vMeshMaxLocal.y) swap(m_vMeshMinLocal.y, m_vMeshMaxLocal.y);
	if (m_vMeshMinLocal.z > m_vMeshMaxLocal.z) swap(m_vMeshMinLocal.z, m_vMeshMaxLocal.z);

	return hr;
}


HRESULT CMesh::Create_Index(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC IDDesc;
	IDDesc.ByteWidth = m_iIndexStride * m_iIndicesCount;
	IDDesc.Usage = D3D11_USAGE_DEFAULT;
	IDDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IDDesc.CPUAccessFlags = 0;
	IDDesc.MiscFlags = 0;
	IDDesc.StructureByteStride = m_iIndexStride;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_indices.data();
	HRESULT hr = pDevice->CreateBuffer(&IDDesc, &subData, &m_pIB);

	vector<_uint>v = {};
	m_indices.swap(v);
	return hr;

}

void CMesh::Create_BoneMinMax(CSkeleton* pSkeleton)
{
	if (!pSkeleton) return;

	m_vMeshMinLocal = { FLT_MAX,FLT_MAX ,FLT_MAX };
	m_vMeshMaxLocal = { -FLT_MAX,-FLT_MAX ,-FLT_MAX };
	_float4x4 root = pSkeleton->Get_TransformationMatrix(0);
	_matrix rootToModel = XMLoadFloat4x4(&root);

	for (const auto& v : m_Skined)
	{
		XMVECTOR pos = XMVector3TransformCoord(XMLoadFloat3(&v.vPosition), rootToModel);

		XMFLOAT3 out;
		XMStoreFloat3(&out, pos);

		m_vMeshMinLocal.x = min(m_vMeshMinLocal.x, out.x);
		m_vMeshMinLocal.y = min(m_vMeshMinLocal.y, out.y);
		m_vMeshMinLocal.z = min(m_vMeshMinLocal.z, out.z);

		m_vMeshMaxLocal.x = max(m_vMeshMaxLocal.x, out.x);
		m_vMeshMaxLocal.y = max(m_vMeshMaxLocal.y, out.y);
		m_vMeshMaxLocal.z = max(m_vMeshMaxLocal.z, out.z);
	}
	vector<VTXSKINMESH>().swap(m_Skined);
}

void CMesh::Render_GUI()
{
	ImGui::Text(m_VIKey.c_str());
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ifstream& ifs, MESH_TYPE eType)
{
  	CMesh* instance = new CMesh();
	if (FAILED(instance->Initialize_From_File(pDevice, ifs, eType))) {
		Safe_Release(instance);
	}
	return instance;
}

void CMesh::Free()
{
	__super::Free();

	vector<_uint>i = {};
	m_indices.swap(i);
}
