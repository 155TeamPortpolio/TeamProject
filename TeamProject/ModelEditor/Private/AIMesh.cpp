#include "pch.h"
#include "AIMesh.h"
#include "AISkeleton.h"
#include "AIBone.h"
#include "GameInstance.h"

CAIMesh::CAIMesh()
    : CMesh{"AIMesh"}
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
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

    HRESULT hr = MESH_TYPE::NONANIM == _eMeshType ? 
        Ready_VertexBuffer_For_NonAnim(_pAIMesh) :Ready_VertexBuffer_For_Anim(_pAIMesh, _pSkeleton);
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
    m_iVertexStride = sizeof(VTXMESH);

    D3D11_BUFFER_DESC           VBDesc{};
    VBDesc.ByteWidth = m_iVerticesCount * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iVerticesCount];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iVerticesCount);

    for (_uint i = 0; i < m_iVerticesCount; i++)
    {
        memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
        memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
        if (nullptr != _pAIMesh->mTextureCoords[0])
            memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
        memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
        memcpy(&pVertices[i].vBinormal, &_pAIMesh->mTangents[i], sizeof(_float3));
    }

    //저장용
    m_Meshes.reserve(m_iVerticesCount);
    for (_uint i = 0; i < m_iVerticesCount; i++) {
        m_Meshes.push_back(pVertices[i]);
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CAIMesh::Ready_VertexBuffer_For_Anim(const aiMesh* _pAIMesh, class CAISkeleton* _pSkeleton)
{
    if (nullptr == _pSkeleton)
        return E_FAIL;

    m_iVertexStride = sizeof(VTXSKINMESH);

    D3D11_BUFFER_DESC VBDesc{};
    VBDesc.ByteWidth = m_iVerticesCount * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXSKINMESH* pVertices = new VTXSKINMESH[m_iVerticesCount];
    ZeroMemory(pVertices, sizeof(VTXSKINMESH) * m_iVerticesCount);

    //정점저장
    for (_uint i = 0; i < m_iVerticesCount; i++)
    {
        memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
        memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
        if (nullptr != _pAIMesh->mTextureCoords[0]) {
            memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
            memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
            memcpy(&pVertices[i].vBinormal, &_pAIMesh->mBitangents[i], sizeof(_float3));
        }
    }

    _uint NumBones = _pAIMesh->mNumBones;

    for (_uint i = 0; i < NumBones; i++)
    {
        aiBone* pAIBone = _pAIMesh->mBones[i];
        if (nullptr == pAIBone)
            return E_FAIL;

        string  BoneName = pAIBone->mName.C_Str();

        _uint iBoneIndex = {};
        if (false == _pSkeleton->Find_BoneIndex(pAIBone->mName.data, &iBoneIndex))
            return E_FAIL;

        for (_uint j = 0; j < pAIBone->mNumWeights; j++)
        {
            aiVertexWeight AIWeight = pAIBone->mWeights[j];

            if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
            }
            else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.y = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
            }
            else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.z = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
            }
            else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.w = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
            }
        }
    }

    if (0 == NumBones) {
        const CAIBone* pBone = _pSkeleton->Find_Bone(_pAIMesh->mName.data);

        if (nullptr != pBone) {
            _uint iBoneIndex = {};
            _pSkeleton->Find_BoneIndex(_pAIMesh->mName.data, &iBoneIndex);

            for (_uint i = 0; i < m_iVerticesCount; ++i)
            {
                pVertices[i].vBlendIndex.x = 0;
                pVertices[i].vBlendWeight.x = 1.0f;
            }
        }
    }

    m_SkinMeshes.reserve(m_iVerticesCount);
    for (_uint i = 0; i < m_iVerticesCount; i++)
    {
        m_SkinMeshes.push_back(pVertices[i]);
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    if (0 == NumBones)
    {
        _int BoneIndex = _pSkeleton->Find_BoneIndexByName(m_VIKey);
        _float4x4       OffsetMatrix;
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
        m_BoneIndices.push_back(BoneIndex);
    }


    return S_OK;
}

CAIMesh* CAIMesh::Create(MESH_TYPE _eType, const aiMesh* _pAIMesh, CAISkeleton* _pSkeleton)
{
    CAIMesh* pInstance = new CAIMesh();

    if (FAILED(pInstance->Initialize(_pAIMesh, _eType, _pSkeleton))) {
        MSG_BOX("Create Failed : Engine | CAIMesh");
        return nullptr;
    }

    return pInstance;
}

void CAIMesh::Free()
{
    __super::Free();
}

