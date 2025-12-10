#include "VI_Cube.h"

CVI_Cube::CVI_Cube(const string& bufferID)
    :CVIBuffer{ bufferID }
{
}

CVI_Cube::CVI_Cube(const CVI_Cube& rhs)
    : CVIBuffer(rhs)
{
}

CVI_Cube::~CVI_Cube()
{
}

HRESULT CVI_Cube::Initialize(ID3D11Device* pDevice)
{
	m_iVertexBufferCount = 1;
	m_iVerticesCount = 8;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iIndicesCount = 36;
	m_iIndexStride = 2; //byte
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (FAILED(Create_Vertex(pDevice)))
		return E_FAIL;

	if (FAILED(Create_Index(pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVI_Cube::Create_Vertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* m_VBContainer = new VTXPOSTEX[m_iVerticesCount];
	ZeroMemory(m_VBContainer, m_iVertexStride * m_iVerticesCount);

	_float s = 0.5;

	m_VBContainer[0].vPosition = _float3(-s, s, -s);		   
	//VB[0].vTexcoord = VB[0].vPosition;
																				 
	m_VBContainer[1].vPosition = _float3(s, s, -s);
	//VB[1].vTexcoord = VB[1].vPosition;
																				 
	m_VBContainer[2].vPosition = _float3(s, -s, -s);
	//VB[2].vTexcoord = VB[2].vPosition;
																				 
	m_VBContainer[3].vPosition = _float3(-s, -s, -s);
	//VB[3].vTexcoord = VB[3].vPosition;

	m_VBContainer[4].vPosition = _float3(-s, s, s);
	//VB[4].vTexcoord = VB[4].vPosition;

	m_VBContainer[5].vPosition = _float3(s, s, s);
	//VB[5].vTexcoord = VB[5].vPosition;

	m_VBContainer[6].vPosition = _float3(s, -s, s);
	//VB[6].vTexcoord = VB[6].vPosition;

	m_VBContainer[7].vPosition = _float3(-s, -s, s);
	//VB[7].vTexcoord = VB[7].vPosition;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_VBContainer;

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	Safe_Delete_Array(m_VBContainer);
	return hr;

}

HRESULT CVI_Cube::Create_Index(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC IDDesc;
	IDDesc.ByteWidth = m_iIndexStride * m_iIndicesCount;
	IDDesc.Usage = D3D11_USAGE_DEFAULT;
	IDDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IDDesc.CPUAccessFlags = 0;
	IDDesc.MiscFlags = 0;
	IDDesc.StructureByteStride = m_iIndexStride;

	_ushort* m_IBContainer = new _ushort[m_iIndicesCount];
	ZeroMemory(m_IBContainer, m_iIndexStride * m_iIndicesCount);
	int i = 0;
	m_IBContainer[i++] = 1;		 m_IBContainer[i++] = 5;    m_IBContainer[i++] = 6;    m_IBContainer[i++] = 1;    m_IBContainer[i++] = 6;    m_IBContainer[i++] = 2;
	m_IBContainer[i++] = 4;		 m_IBContainer[i++] = 0;    m_IBContainer[i++] = 3;    m_IBContainer[i++] =4;    m_IBContainer[i++] = 3;    m_IBContainer[i++] = 7;
	m_IBContainer[i++] = 4;		 m_IBContainer[i++] = 5;    m_IBContainer[i++] = 1;    m_IBContainer[i++] =4;    m_IBContainer[i++] = 1;    m_IBContainer[i++] = 0;
	m_IBContainer[i++] = 3;		 m_IBContainer[i++] = 2;    m_IBContainer[i++] = 6;    m_IBContainer[i++] =3;    m_IBContainer[i++] = 6;    m_IBContainer[i++] = 7;
	m_IBContainer[i++] = 5;		 m_IBContainer[i++] = 4;    m_IBContainer[i++] = 7;    m_IBContainer[i++] =5;    m_IBContainer[i++] = 7;    m_IBContainer[i++] = 6;
	m_IBContainer[i++] = 0;		 m_IBContainer[i++] = 1;    m_IBContainer[i++] = 2;    m_IBContainer[i++] = 0;    m_IBContainer[i++] = 2;    m_IBContainer[i++] = 3;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_IBContainer;

	HRESULT hr = pDevice->CreateBuffer(&IDDesc, &subData, &m_pIB);

	Safe_Delete_Array(m_IBContainer);
	return hr;

}

CVI_Cube* CVI_Cube::Create(ID3D11Device* pDevice, const string& bufferID)
{
    CVI_Cube* instance = new CVI_Cube(bufferID);
    if (FAILED(instance->Initialize(pDevice))) {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(instance);
    }
    return instance;
}

void CVI_Cube::Free()
{
	__super::Free();
}
