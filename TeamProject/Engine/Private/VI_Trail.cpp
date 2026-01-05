#include "Engine_Defines.h"
#include "VI_Trail.h"
#include "GameInstance.h"

CVI_Trail::CVI_Trail(const string& bufferID)
	:CVIBuffer(bufferID)
{
}

CVI_Trail::CVI_Trail(const CVI_Trail& rhs)
	:CVIBuffer(rhs)
{
}

CVI_Trail::~CVI_Trail()
{
}

HRESULT CVI_Trail::Initialize(ID3D11Device* pDevice)
{
	m_ElementCount = VTXTRAIL::iElementCount;
	m_ElementKey = VTXTRAIL::Key;
	m_ElementDesc = VTXTRAIL::Elements;

	m_iVertexBufferCount = 1;
	m_iVerticesCount = g_iMaxNumTrailPoints * 2;
	m_iVertexStride = sizeof(VTXTRAIL);

	m_iIndicesCount = (g_iMaxNumTrailPoints - 1) * 2;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (FAILED(Create_Vertex(pDevice)))
		return E_FAIL;

	if (FAILED(Create_Index(pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVI_Trail::Bind_Buffer(ID3D11DeviceContext* pContext)
{
	if (m_iCurrPointCount < 2)
		return S_OK;

	ID3D11Buffer* pVertexBuffers[] = { m_pVB };
	_uint  pVertexStride[] = { m_iVertexStride };
	_uint  pVertexOffset[] = { 0 };

	pContext->IASetVertexBuffers(0, m_iVertexBufferCount, pVertexBuffers, pVertexStride, pVertexOffset);
	pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVI_Trail::Render(ID3D11DeviceContext* pContext)
{
	if (m_iCurrPointCount < 2)
		return S_OK;

	pContext->DrawIndexed(m_iCurrPointCount * 6, 0, 0);

	return S_OK;
}

void CVI_Trail::Update_Vertices(VTXTRAIL* pVertices, _uint iCount)
{
	m_iCurrPointCount = iCount;
	if (m_iCurrPointCount < 2)
		return;

	_uint iVerticesCont = iCount * 2;

	auto pContext = CGameInstance::GetInstance()->Get_Context();

	D3D11_MAPPED_SUBRESOURCE mapSubResource{};
	pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubResource);
	memcpy_s(mapSubResource.pData, sizeof(m_iVertexStride * m_iVerticesCount), pVertices, sizeof(m_iVertexStride * iVerticesCont));
	pContext->Unmap(m_pVB, 0);
}

HRESULT CVI_Trail::Create_Vertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = 0;

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB);

	return hr;
}

HRESULT CVI_Trail::Create_Index(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iIndicesCount;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData{};
	//subData.pSysMem = 

	HRESULT hr = pDevice->CreateBuffer(&IBDesc, &subData, &m_pIB);

	return hr;
}

CVI_Trail* CVI_Trail::Create(ID3D11Device* pDevice, const string& bufferID)
{
	CVI_Trail* instance = new CVI_Trail(bufferID);
	if (FAILED(instance->Initialize(pDevice))) {
		MSG_BOX("Failed to Created : CVI_Trail");
		Safe_Release(instance);
	}
	return instance;
}

void CVI_Trail::Free()
{
	__super::Free();
}
