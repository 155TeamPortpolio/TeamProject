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
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_ElementCount = eType == MESH_TYPE::ANIM ? VTXSKINMESH::iElementCount : VTXMESH::iElementCount;
	m_ElementKey = eType == MESH_TYPE::ANIM ? VTXSKINMESH::Key : VTXMESH::Key;
	m_ElementDesc = eType == MESH_TYPE::ANIM ? VTXSKINMESH::Elements : VTXMESH::Elements;
	m_OffsetCount = infoHeader.offsetCount;

	HRESULT hr = eType == MESH_TYPE::ANIM ? Create_AnimateVertex(pDevice, ifs) : Create_StaticVertex(pDevice, ifs);
	if (FAILED(hr))
		return E_FAIL;

	m_indices.resize(infoHeader.IndicesCount);
	ifs.read(reinterpret_cast<char*>(m_indices.data()), m_iIndicesCount * m_iIndexStride);

	if (eType == MESH_TYPE::ANIM)
	{
		hr = Finalize_AnimateVertexVB(pDevice); 
		if (FAILED(hr))
			return E_FAIL;
	}

	hr = Create_Index(pDevice);
	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Create_AnimateVertex(ID3D11Device* pDevice, ifstream& ifs)
{
	vector<VTXSKINMESH> vertices;
	vertices.resize(m_iVerticesCount);

	ifs.read(reinterpret_cast<char*>(vertices.data()), m_iVerticesCount * m_iVertexStride);

	for (size_t indexOffset = 0; indexOffset < m_OffsetCount; ++indexOffset)
	{
		MESH_OFFSET offset;
		ifs.read(reinterpret_cast<char*>(&offset), sizeof(MESH_OFFSET));
		m_MeshOffset.emplace(offset.BoneIndex, offset.offsetMat);
	}

	m_Skined = std::move(vertices);
	return S_OK;
}

HRESULT CMesh::Finalize_AnimateVertexVB(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = m_iVertexStride;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = m_Skined.data();

	Safe_Release(m_pVB);
	return pDevice->CreateBuffer(&vbDesc, &subData, &m_pVB);
}

HRESULT CMesh::Create_StaticVertex(ID3D11Device* pDevice, ifstream& ifs)
{
	m_ElementCount = VTXMESH::iElementCount;
	m_ElementKey = VTXMESH::Key;
	m_ElementDesc = VTXMESH::Elements;
	m_StaticVertex.resize(m_iVerticesCount);

	ifs.read(reinterpret_cast<char*>(m_StaticVertex.data()), m_iVerticesCount * m_iVertexStride);
	for (size_t i = 0; i < m_OffsetCount; i++)
	{
		MESH_OFFSET offset;
		ifs.read(reinterpret_cast<char*>(&offset), sizeof(MESH_OFFSET));
		m_MeshOffset.emplace(offset.BoneIndex, offset.offsetMat);
	}
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_StaticVertex.data();

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	m_vMeshMinLocal = { FLT_MAX,FLT_MAX ,FLT_MAX };
	m_vMeshMaxLocal = { -FLT_MAX,-FLT_MAX ,-FLT_MAX };

	for (const auto& vertex : m_StaticVertex) {
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

	return hr;
}

_float4x4 CMesh::Get_MeshOffset(_uint boneIndex)
{
	auto iter = m_MeshOffset.find(boneIndex);
	if (iter != m_MeshOffset.end()) {
		return iter->second;
	}
	else {
		_float4x4 identity;
		XMStoreFloat4x4(&identity, XMMatrixIdentity());
		return identity;
	}
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

HRESULT CMesh::BakeSkinRemapAndRebuildVB(ID3D11Device* pDevice, _uint skeletonBoneCount)
{
	if (!pDevice) return E_FAIL;
	if (m_Skined.empty()) return S_OK;
	if (m_indices.empty()) return E_FAIL;
	if (skeletonBoneCount == 0) return E_FAIL;

	BuildUsedBonesAndRemap(skeletonBoneCount);
	RemapVertexBlendIndices();

	HRESULT hr = Finalize_AnimateVertexVB(pDevice);
	return hr;
}

void CMesh::ExpandBox(MINMAX_BOX& b, const _float3& p)
{
	b.ExpandBox(p);
}

static inline bool HasWeight(const XMFLOAT4& weight, int lane)
{
	const float* w = reinterpret_cast<const float*>(&weight);
	return w[lane] > 1e-6f;
}

static inline uint32_t GetBlendIndexLane(const XMUINT4& blendIndex, int lane)
{
	const uint32_t* idx = reinterpret_cast<const uint32_t*>(&blendIndex);
	return idx[lane];
}

void CMesh::BuildUsedBonesAndRemap(_uint skeletonBoneCount)
{
	m_UsedBones.clear();
	m_GlobalToLocal.assign(skeletonBoneCount, (uint16_t)0xFFFF);

	vector<uint8_t> usedFlag;
	usedFlag.assign(skeletonBoneCount, 0);

	for (_uint indexValue : m_indices)
	{
		const VTXSKINMESH& vertex = m_Skined[indexValue];

		for (int lane = 0; lane < 4; ++lane)
		{
			if (!HasWeight(vertex.vBlendWeight, lane))
				continue;

			_uint globalBoneIndex = GetBlendIndexLane(vertex.vBlendIndex, lane);
			if (globalBoneIndex >= skeletonBoneCount)
				continue;

			usedFlag[globalBoneIndex] = 1;
		}
	}

	for (_uint globalBoneIndex = 0; globalBoneIndex < skeletonBoneCount; ++globalBoneIndex)
	{
		if (!usedFlag[globalBoneIndex])
			continue;

		uint16_t localIndex = (uint16_t)m_UsedBones.size();
		m_UsedBones.push_back((uint16_t)globalBoneIndex);
		m_GlobalToLocal[globalBoneIndex] = localIndex;
	}
}

void CMesh::RemapVertexBlendIndices()
{
	for (VTXSKINMESH& vertex : m_Skined)
	{
		uint32_t* idx = reinterpret_cast<uint32_t*>(&vertex.vBlendIndex);
		const float* w = reinterpret_cast<const float*>(&vertex.vBlendWeight);

		for (int lane = 0; lane < 4; ++lane)
		{
			if (w[lane] <= 1e-6f)
			{
				idx[lane] = 0;
				continue;
			}

			uint32_t globalBoneIndex = idx[lane];
			uint16_t localIndex = (globalBoneIndex < m_GlobalToLocal.size()) ? m_GlobalToLocal[globalBoneIndex] : (uint16_t)0xFFFF;

			idx[lane] = (localIndex == (uint16_t)0xFFFF) ? 0 : (uint32_t)localIndex;
		}
	}
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

	vector<_uint>().swap(m_indices);
	vector<VTXMESH>().swap(m_StaticVertex);
	vector<VTXSKINMESH>().swap(m_Skined);
}
