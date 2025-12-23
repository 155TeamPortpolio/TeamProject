#include "pch.h"
#include "AIMesh.h"
#include "AISkeleton.h"
#include "AIBone.h"
#include "GameInstance.h"

CAIMesh::CAIMesh()
	: CMesh{ "AIMesh" }
{
}

CAIMesh::CAIMesh(const string& ModelKey)
	:CMesh(ModelKey)
{
}

HRESULT CAIMesh::Initialize(const aiMesh* _pAIMesh, MESH_TYPE _eMeshType, CAISkeleton* _pSkeleton)
{
	m_VIKey = _pAIMesh->mName.C_Str(); 
	m_MaterialIndex = _pAIMesh->mMaterialIndex;

	m_iVertexBufferCount = 1;
	m_iVerticesCount = _pAIMesh->mNumVertices;

	m_iIndicesCount = _pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pSkeleton = _pSkeleton;
	m_ElementCount = _eMeshType == MESH_TYPE::ANIM ? VTXSKINMESH::iElementCount : VTXMESH::iElementCount;
	m_ElementKey = _eMeshType == MESH_TYPE::ANIM ? VTXSKINMESH::Key : VTXMESH::Key;
	m_ElementDesc = _eMeshType == MESH_TYPE::ANIM ? VTXSKINMESH::Elements : VTXMESH::Elements;
	m_iVertexStride = _eMeshType == MESH_TYPE::ANIM ? sizeof(VTXSKINMESH) : sizeof(VTXMESH);
	Safe_AddRef(m_pSkeleton);

#pragma region VERTEX_BUFFER
	if (_eMeshType == MESH_TYPE::NONANIM && m_pSkeleton)
	{
		BoneIndex = m_pSkeleton->Find_BoneIndexByName(m_VIKey);
		isRigid = (BoneIndex >= 0);
	}

	HRESULT hr = MESH_TYPE::NONANIM == _eMeshType ?
		Ready_VertexBuffer_For_NonAnim(_pAIMesh) : Ready_VertexBuffer_For_Anim(_pAIMesh, _pSkeleton);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER 
	D3D11_BUFFER_DESC           IBDesc{};
	IBDesc.ByteWidth = m_iIndicesCount * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iIndicesCount];
	ZeroMemory(pIndices, sizeof(_uint) * m_iIndicesCount);

	_uint       iNumIndices = {};

	for (_uint i = 0; i < _pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[2];
	}

	//저장용
	m_Indices.reserve(iNumIndices);
	for (_uint i = 0; i < iNumIndices; i++)
	{
		m_Indices.push_back(pIndices[i]);
	}

	D3D11_SUBRESOURCE_DATA      IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&IBDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CAIMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* _pAIMesh)
{

	D3D11_BUFFER_DESC           VBDesc{};
	VBDesc.ByteWidth = m_iVerticesCount * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;
	m_Meshes.reserve(m_iVerticesCount);
	m_vMeshMinLocal = { FLT_MAX, FLT_MAX, FLT_MAX };
	m_vMeshMaxLocal = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (_uint i = 0; i < m_iVerticesCount; i++)
	{
		VTXMESH mesh = {};
		memcpy(&mesh.vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&mesh.vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		if (_pAIMesh->mNumUVComponents[0] > 0) {
			memcpy(&mesh.vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
			memcpy(&mesh.vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
			memcpy(&mesh.vBinormal, &_pAIMesh->mBitangents[i], sizeof(_float3));
		}
		m_vMeshMinLocal.x = min(m_vMeshMinLocal.x, mesh.vPosition.x);
		m_vMeshMinLocal.y = min(m_vMeshMinLocal.y, mesh.vPosition.y);
		m_vMeshMinLocal.z = min(m_vMeshMinLocal.z, mesh.vPosition.z);

		m_vMeshMaxLocal.x = max(m_vMeshMaxLocal.x, mesh.vPosition.x);
		m_vMeshMaxLocal.y = max(m_vMeshMaxLocal.y, mesh.vPosition.y);
		m_vMeshMaxLocal.z = max(m_vMeshMaxLocal.z, mesh.vPosition.z);

		m_Meshes.push_back(mesh);
	}

	if (m_vMeshMinLocal.x > m_vMeshMaxLocal.x) swap(m_vMeshMinLocal.x, m_vMeshMaxLocal.x);
	if (m_vMeshMinLocal.y > m_vMeshMaxLocal.y) swap(m_vMeshMinLocal.y, m_vMeshMaxLocal.y);
	if (m_vMeshMinLocal.z > m_vMeshMaxLocal.z) swap(m_vMeshMinLocal.z, m_vMeshMaxLocal.z);

	D3D11_SUBRESOURCE_DATA      VertexInitialData{};
	VertexInitialData.pSysMem = m_Meshes.data();
	if (FAILED(CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAIMesh::Ready_VertexBuffer_For_Anim(const aiMesh* _pAIMesh, class CAISkeleton* _pSkeleton)
{
	if (nullptr == _pSkeleton)
		return E_FAIL;

	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	m_SkinMeshes.reserve(m_iVerticesCount);
	m_vMeshMinLocal = { FLT_MAX, FLT_MAX, FLT_MAX };
	m_vMeshMaxLocal = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	//정점저장
	for (_uint i = 0; i < m_iVerticesCount; i++)
	{
		VTXSKINMESH mesh = {};
		memcpy(&mesh.vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&mesh.vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		if (_pAIMesh->mNumUVComponents[0] > 0) {
			memcpy(&mesh.vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
			memcpy(&mesh.vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
			memcpy(&mesh.vBinormal, &_pAIMesh->mBitangents[i], sizeof(_float3));
		}
		m_vMeshMinLocal.x = min(m_vMeshMinLocal.x, mesh.vPosition.x);
		m_vMeshMinLocal.y = min(m_vMeshMinLocal.y, mesh.vPosition.y);
		m_vMeshMinLocal.z = min(m_vMeshMinLocal.z, mesh.vPosition.z);

		m_vMeshMaxLocal.x = max(m_vMeshMaxLocal.x, mesh.vPosition.x);
		m_vMeshMaxLocal.y = max(m_vMeshMaxLocal.y, mesh.vPosition.y);
		m_vMeshMaxLocal.z = max(m_vMeshMaxLocal.z, mesh.vPosition.z);
		m_SkinMeshes.push_back(mesh);
	}

	_uint NumBones = _pAIMesh->mNumBones;

	for (_uint i = 0; i < NumBones; i++)
	{
		aiBone* pAIBone = _pAIMesh->mBones[i];
		if (nullptr == pAIBone)
			return E_FAIL;

		string BoneName = pAIBone->mName.C_Str();
		BoneIndex = m_pSkeleton->Find_BoneIndexByName(BoneName);

		if (BoneIndex < 0) continue; 
		m_BoneIndices.push_back(BoneIndex);

		_float4x4 OffsetMatrix = {};
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
		_pSkeleton->Set_Offset(BoneIndex, OffsetMatrix);
		m_MeshOffset.emplace((_uint)BoneIndex, OffsetMatrix);

		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight     AIWeight = pAIBone->mWeights[j];

			if (0.0f == m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.x)
			{
				m_SkinMeshes[AIWeight.mVertexId].vBlendIndex.x = BoneIndex;
				m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
			}

			else if (0.0f == m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.y)
			{
				m_SkinMeshes[AIWeight.mVertexId].vBlendIndex.y = BoneIndex;
				m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
			}

			else if (0.0f == m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.z)
			{
				m_SkinMeshes[AIWeight.mVertexId].vBlendIndex.z = BoneIndex;
				m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
			}

			else if (0.0f == m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.w)
			{
				m_SkinMeshes[AIWeight.mVertexId].vBlendIndex.w = BoneIndex;
				m_SkinMeshes[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
			}
		}
	}

	if (0 == NumBones)
	{
		_int BoneIndex = m_pSkeleton->Find_BoneIndexByName(m_VIKey);
		_float4x4       OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_BoneIndices.push_back(BoneIndex);
	}

	D3D11_SUBRESOURCE_DATA      VertexInitialData{};
	VertexInitialData.pSysMem = m_SkinMeshes.data();

	if (FAILED(CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

void CAIMesh::Save_File(ofstream& ofs, _fmatrix PreTransform)
{
	MESH_INFO_HEADER infoHeader = {};
	infoHeader.BoneCount = m_BoneIndices.size();
	infoHeader.IndicesCount = m_iIndicesCount;
	infoHeader.VerticesCount = m_iVerticesCount;
	strcpy_s(infoHeader.MeshName, m_VIKey.c_str());
	infoHeader.MaterialIndex = m_MaterialIndex;
	infoHeader.offsetCount = m_MeshOffset.size();
	ofs.write(reinterpret_cast<const char*>(&infoHeader), sizeof(MESH_INFO_HEADER));

	if (m_iVertexStride == sizeof(VTXSKINMESH)) {
		for (VTXSKINMESH& vertex : m_SkinMeshes) {
			ofs.write(reinterpret_cast<const char*>(&vertex), sizeof(VTXSKINMESH));
		}
		for (auto& pair : m_MeshOffset)
		{
			MESH_OFFSET offset;
			offset.BoneIndex = pair.first;
			offset.offsetMat = pair.second;
			ofs.write(reinterpret_cast<const char*>(&offset), sizeof(MESH_OFFSET));
		}
	}

	else if (m_iVertexStride == sizeof(VTXMESH)) {
		for (VTXMESH& vertex : m_Meshes) {
			XMStoreFloat3(&vertex.vPosition, XMVector3TransformCoord(XMLoadFloat3(&vertex.vPosition), PreTransform));
			XMStoreFloat3(&vertex.vNormal, XMVector3TransformNormal(XMLoadFloat3(&vertex.vNormal), PreTransform));
			XMStoreFloat3(&vertex.vTangent, XMVector3TransformNormal(XMLoadFloat3(&vertex.vTangent), PreTransform));
			XMStoreFloat3(&vertex.vBinormal, XMVector3TransformNormal(XMLoadFloat3(&vertex.vBinormal), PreTransform));

			ofs.write(reinterpret_cast<const char*>(&vertex), sizeof(VTXMESH));
		}
	}

	for (_uint indices : m_Indices)
		ofs.write(reinterpret_cast<const char*>(&indices), sizeof(_uint));
}


CAIMesh* CAIMesh::Create(MESH_TYPE _eType, const aiMesh* _pAIMesh, CAISkeleton* _pSkeleton)
{
	CAIMesh* pInstance = new CAIMesh();
	if (FAILED(pInstance->Initialize(_pAIMesh, _eType, _pSkeleton))) {
		MSG_BOX("Create Failed : Engine | CAIMesh");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CAIMesh::Free()
{
	__super::Free();
	Safe_Release(m_pSkeleton);
}

