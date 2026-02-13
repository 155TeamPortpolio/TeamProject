#include "Engine_Defines.h"
#include "VI_Tessellation.h"

CVI_Tessellation::CVI_Tessellation(const string& bufferID)
	:CVIBuffer{ bufferID }
{
}

CVI_Tessellation::CVI_Tessellation(const CVI_Tessellation& rhs)
	: CVIBuffer(rhs)
{
}

CVI_Tessellation::~CVI_Tessellation()
{
}

HRESULT CVI_Tessellation::Initialize(ID3D11Device* pDevice, _uint iGridSize, _float fTotalSize)
{
    m_iGridSize = iGridSize;

    m_ElementCount = VTXTESS::iElementCount;
    m_ElementKey = VTXTESS::Key;
    m_ElementDesc = VTXTESS::Elements;
    m_iVertexBufferCount = 1;
    m_iVerticesCount = (iGridSize + 1) * (iGridSize + 1);
    m_iVertexStride = sizeof(VTXTESS);
    m_iIndicesCount = iGridSize * iGridSize * 6;
    m_iIndexStride = 4;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

    if (FAILED(Create_Vertex(pDevice, fTotalSize)))
        return E_FAIL;
    if (FAILED(Create_Index(pDevice)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVI_Tessellation::Create_Vertex(ID3D11Device* pDevice, _float fTotalSize)
{
    D3D11_BUFFER_DESC VBDesc;
    VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXTESS* pVertices = new VTXTESS[m_iVerticesCount];
    ZeroMemory(pVertices, m_iVertexStride * m_iVerticesCount);

    _float fHalf = fTotalSize * 0.5f;
    _float fStep = fTotalSize / (_float)m_iGridSize;

    for (_uint z = 0; z <= m_iGridSize; ++z)
    {
        for (_uint x = 0; x <= m_iGridSize; ++x)
        {
            _uint idx = z * (m_iGridSize + 1) + x;

            pVertices[idx].vPosition = _float3(
                -fHalf + x * fStep,
                0.f,
                -fHalf + z * fStep
            );

            pVertices[idx].vTexcoord = _float2(
                (_float)x / (_float)m_iGridSize,
                (_float)z / (_float)m_iGridSize
            );
        }
    }

    D3D11_SUBRESOURCE_DATA subData;
    subData.pSysMem = pVertices;
    HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);
    Safe_Delete_Array(pVertices);
    return hr;
}

HRESULT CVI_Tessellation::Create_Index(ID3D11Device* pDevice)
{
    D3D11_BUFFER_DESC IDDesc;
    IDDesc.ByteWidth = m_iIndexStride * m_iIndicesCount;
    IDDesc.Usage = D3D11_USAGE_DEFAULT;
    IDDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IDDesc.CPUAccessFlags = 0;
    IDDesc.MiscFlags = 0;
    IDDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iIndicesCount];
    ZeroMemory(pIndices, m_iIndexStride * m_iIndicesCount);

    _uint idx = 0;
    for (_uint z = 0; z < m_iGridSize; ++z)
    {
        for (_uint x = 0; x < m_iGridSize; ++x)
        {
            _uint tl = z * (m_iGridSize + 1) + x;
            _uint tr = tl + 1;
            _uint bl = (z + 1) * (m_iGridSize + 1) + x;
            _uint br = bl + 1;

            pIndices[idx++] = tl;
            pIndices[idx++] = tr;
            pIndices[idx++] = bl;

            pIndices[idx++] = tr;
            pIndices[idx++] = br;
            pIndices[idx++] = bl;
        }
    }

    D3D11_SUBRESOURCE_DATA subData;
    subData.pSysMem = pIndices;
    HRESULT hr = pDevice->CreateBuffer(&IDDesc, &subData, &m_pIB);
    Safe_Delete_Array(pIndices);
    return hr;
}

CVI_Tessellation* CVI_Tessellation::Create(ID3D11Device* pDevice, const string& bufferID, _uint iGridSize, _float fTotalSize)
{
	CVI_Tessellation* instance = new CVI_Tessellation(bufferID);
	if (FAILED(instance->Initialize(pDevice, iGridSize, fTotalSize))) {
		MSG_BOX("Failed to Created : CVI_Tessellation");
		Safe_Release(instance);
	}
	return instance;
}

void CVI_Tessellation::Free()
{
	__super::Free();
}
