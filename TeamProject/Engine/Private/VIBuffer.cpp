#include "Engine_Defines.h"
#include "VIBuffer.h"

CVIBuffer::CVIBuffer(const string& ModelKey)
	:m_VIKey{ ModelKey }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	:m_pVB{rhs.m_pVB}
	, m_pIB{rhs.m_pIB}
	, m_iVertexBufferCount  { rhs.m_iVertexBufferCount }
	, m_iVerticesCount  { rhs.m_iVerticesCount }
	, m_iVertexStride  { rhs.m_iVertexStride }
	, m_iIndicesCount  { rhs.m_iIndicesCount }
	, m_iIndexStride  { rhs.m_iIndexStride }
	, m_eIndexFormat{ rhs.m_eIndexFormat }
	, m_ePrimitive{ rhs.m_ePrimitive }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

CVIBuffer::~CVIBuffer()
{
}


HRESULT CVIBuffer::Initialize(ID3D11Device* pDevice)
{
	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffer(ID3D11DeviceContext* pContext)
{
	ID3D11Buffer* pVertexBuffers[] = {m_pVB};
	_uint  pVertexStride[] = {m_iVertexStride};
	_uint  pVertexOffset[] = {0};

	pContext->IASetVertexBuffers(0, m_iVertexBufferCount, pVertexBuffers, pVertexStride, pVertexOffset);
	pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVIBuffer::Render(ID3D11DeviceContext* pContext)
{
	pContext->DrawIndexed(m_iIndicesCount, 0, 0);
	return S_OK;
}

void CVIBuffer::Render_GUI()
{
}

HRESULT CVIBuffer::Create_Vertex(ID3D11Device* pDevice)
{
	return S_OK;
}

HRESULT CVIBuffer::Create_Index(ID3D11Device* pDevice)
{
	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
