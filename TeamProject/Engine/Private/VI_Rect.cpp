#include "VI_Rect.h"

CVI_Rect::CVI_Rect(const string& bufferID)
	:CVIBuffer{ bufferID }
{
}

CVI_Rect::CVI_Rect(const CVI_Rect& rhs)
	: CVIBuffer(rhs)
{
}

CVI_Rect::~CVI_Rect()
{
}

HRESULT CVI_Rect::Initialize(ID3D11Device* pDevice)
{
	m_ElementCount = VTXNORMTEX::iElementCount;
	m_ElementKey = VTXNORMTEX::Key;
	m_ElementDesc = VTXNORMTEX::Elements;
	m_iVertexBufferCount = 1;
	m_iVerticesCount = 4;
	m_iVertexStride = sizeof(VTXNORMTEX);
	m_iIndicesCount = 6;
	m_iIndexStride = 2; //byte
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (FAILED(Create_Vertex(pDevice)))
		return E_FAIL;

	if (FAILED(Create_Index(pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVI_Rect::Create_Vertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXNORMTEX* m_VBContainer = new VTXNORMTEX[m_iVerticesCount];
	ZeroMemory(m_VBContainer, m_iVertexStride * m_iVerticesCount);

	_float s = 0.5;

	m_VBContainer[0].vPosition = _float3(-s, s, 0.f);
	m_VBContainer[0].vTexcoord = _float2(0.f, 0.f);

	m_VBContainer[1].vPosition = _float3(s, s, 0.f);
	m_VBContainer[1].vTexcoord = _float2(1.f, 0.f);

	m_VBContainer[2].vPosition = _float3(s, -s, 0.f);
	m_VBContainer[2].vTexcoord = _float2(1.f, 1.f);

	m_VBContainer[3].vPosition = _float3(-s, -s, 0.f);
	m_VBContainer[3].vTexcoord = _float2(0.f, 1.f);

	_float3 normal = _float3(0.f, 0.f, 1.f);

	m_VBContainer[0].vNormal = normal;
	m_VBContainer[1].vNormal = normal;
	m_VBContainer[2].vNormal = normal;
	m_VBContainer[3].vNormal = normal;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_VBContainer;

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	Safe_Delete_Array(m_VBContainer);
	return hr;
}

HRESULT CVI_Rect::Create_Index(ID3D11Device* pDevice)
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

	m_IBContainer[0] = 0;    m_IBContainer[1] = 1;    m_IBContainer[2] = 2;    m_IBContainer[3] = 0;    m_IBContainer[4] = 2;    m_IBContainer[5] = 3;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_IBContainer;

	HRESULT hr = pDevice->CreateBuffer(&IDDesc, &subData, &m_pIB);

	Safe_Delete_Array(m_IBContainer);
	return hr;
}

CVI_Rect* CVI_Rect::Create(ID3D11Device* pDevice,const string& bufferID)
{
	CVI_Rect* instance = new CVI_Rect(bufferID);
	if (FAILED(instance->Initialize(pDevice))) {
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(instance);
	}
	return instance;
}

void CVI_Rect::Free()
{
	__super::Free();
}
