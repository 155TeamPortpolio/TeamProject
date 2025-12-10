#include "VI_Point.h"

CVI_Point::CVI_Point(const string& bufferID)
	:CVIBuffer{ bufferID }
{
}

CVI_Point::CVI_Point(const CVI_Point& rhs)
	: CVIBuffer(rhs)
{
}

CVI_Point::~CVI_Point()
{
}

HRESULT CVI_Point::Initialize(ID3D11Device* pDevice)
{
	m_ElementCount = VTXPOS::iElementCount;
	m_ElementKey = VTXPOS::Key;
	m_ElementDesc = VTXPOS::Elements;
	m_iVertexBufferCount = 1;
	m_iVerticesCount = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_ePrimitive = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	if (FAILED(Create_Vertex(pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVI_Point::Bind_Buffer(ID3D11DeviceContext* pContext)
{

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB
	};

	_uint			iVertexStrides[] = {
		m_iVertexStride
	};

	_uint			iOffsets[] = {
		0,
	};

	pContext->IASetVertexBuffers(0, m_iVertexBufferCount, pVertexBuffers, iVertexStrides, iOffsets);
	pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVI_Point::Render(ID3D11DeviceContext* pContext)
{
	pContext->Draw(1, 0);
	return S_OK;
}

HRESULT CVI_Point::Create_Vertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* m_VBContainer = new VTXPOS[m_iVerticesCount];
	ZeroMemory(m_VBContainer, m_iVertexStride * m_iVerticesCount);
	m_VBContainer[0].vPosition = _float3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_VBContainer;

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	Safe_Delete_Array(m_VBContainer);
	return hr;
}

CVI_Point* CVI_Point::Create(ID3D11Device* pDevice,const string& bufferID)
{
	CVI_Point* instance = new CVI_Point(bufferID);
	if (FAILED(instance->Initialize(pDevice))) {
		MSG_BOX("Failed to Created : CVI_Point");
		Safe_Release(instance);
	}
	return instance;
}

void CVI_Point::Free()
{
	__super::Free();
}
