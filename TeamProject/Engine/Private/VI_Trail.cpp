#include "Engine_Defines.h"
#include "VI_Trail.h"

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
	m_iVerticesCount = g_iMaxNumTrailPoints;
	m_iVertexStride = sizeof(VTXTRAIL);

	//m_iIndicesCount = ;
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
	return S_OK;
}

HRESULT CVI_Trail::Render(ID3D11DeviceContext* pContext)
{
	return S_OK;
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

	//HRESULT hr = ;

	return S_OK;
}

CVI_Trail* CVI_Trail::Create(ID3D11Device* pDevice, const string& bufferID)
{
	return nullptr;
}

void CVI_Trail::Free()
{
}
