#include "Engine_Defines.h"
#include "VI_InstancePoint.h"

CVI_InstancePoint::CVI_InstancePoint(const string& bufferID)
	:CVIBuffer(bufferID)
{
}

CVI_InstancePoint::CVI_InstancePoint(const CVI_InstancePoint& rhs)
	:CVIBuffer(rhs)
{
}

CVI_InstancePoint::~CVI_InstancePoint()
{
}

HRESULT CVI_InstancePoint::Initialize(ID3D11Device* pDevice)
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

HRESULT CVI_InstancePoint::Bind_Buffer(ID3D11DeviceContext* pContext)
{
	return E_NOTIMPL;
}

HRESULT CVI_InstancePoint::Render(ID3D11DeviceContext* pContext)
{
	return E_NOTIMPL;
}

HRESULT CVI_InstancePoint::Create_Vertex(ID3D11Device* pDevice)
{
	return E_NOTIMPL;
}

CVI_InstancePoint* CVI_InstancePoint::Create(ID3D11Device* pDevice, const string& bufferID)
{
	return nullptr;
}

void CVI_InstancePoint::Free()
{
}
