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
	m_ElementCount = VTX_INSTANCE_POINT_ELEMENT::iElementCount;
	m_ElementKey = VTX_INSTANCE_POINT_ELEMENT::Key;
	m_ElementDesc = VTX_INSTANCE_POINT_ELEMENT::Elements;

	m_iVertexBufferCount = 2;
	m_iVerticesCount = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_ePrimitive = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_iMaxInstancesCount = g_iMaxNumInstances;
	m_iInstanceStride = sizeof(VTX_INSTANCE_POINT);
	m_iNumUsedInstances = 0;

	if (FAILED(Create_Vertex(pDevice)))
		return E_FAIL;

	if (FAILED(Create_InstanceBuffer(pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVI_InstancePoint::Bind_Buffer(ID3D11DeviceContext* pContext)
{
	ID3D11Buffer* pVertexBuffers[2] = { m_pVB, m_pInstanceBuffer };
	_uint  pVertexStrides[2] = { m_iVertexStride, m_iInstanceStride };
	_uint  pVertexOffsets[2] = { 0,0 };

	pContext->IASetVertexBuffers(0, m_iVertexBufferCount, pVertexBuffers, pVertexStrides, pVertexOffsets);
	pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVI_InstancePoint::Render(ID3D11DeviceContext* pContext)
{
	return S_OK;
}

HRESULT CVI_InstancePoint::Create_Vertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iVerticesCount;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* m_VBContainer = new VTXPOS[m_iVerticesCount];
	ZeroMemory(m_VBContainer, m_iVertexStride * m_iVerticesCount);
	m_VBContainer[0].vPosition = _float3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA subData{};
	subData.pSysMem = m_VBContainer;

	HRESULT hr = pDevice->CreateBuffer(&VBDesc, &subData, &m_pVB);

	Safe_Delete_Array(m_VBContainer);
	return hr;
}

HRESULT CVI_InstancePoint::Create_InstanceBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC InstanceDesc{};
	InstanceDesc.ByteWidth = m_iInstanceStride * m_iMaxInstancesCount;
	InstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceDesc.MiscFlags = 0;
	InstanceDesc.StructureByteStride = m_iInstanceStride;

	VTX_INSTANCE_POINT* VBContainer = new VTX_INSTANCE_POINT[m_iMaxInstancesCount];
	ZeroMemory(VBContainer, m_iInstanceStride * m_iMaxInstancesCount);

	D3D11_SUBRESOURCE_DATA subData{};
	subData.pSysMem = VBContainer;

	HRESULT hr = pDevice->CreateBuffer(&InstanceDesc, &subData, &m_pInstanceBuffer);

	Safe_Delete_Array(VBContainer);
	return hr;
}

CVI_InstancePoint* CVI_InstancePoint::Create(ID3D11Device* pDevice, const string& bufferID)
{
	CVI_InstancePoint* instance = new CVI_InstancePoint(bufferID);
	if (FAILED(instance->Initialize(pDevice))) {
		MSG_BOX("Failed to Created : CVI_InstancePoint");
		Safe_Release(instance);
	}
	return instance;
}

void CVI_InstancePoint::Free()
{
	__super::Free();

	Safe_Release(m_pInstanceBuffer);
}

void CVI_InstancePoint::Update_InstanceBuffer(ID3D11DeviceContext* pContext, const VTX_INSTANCE_POINT* instanceData, _uint numInstance)
{
	if (numInstance > m_iMaxInstancesCount)
		numInstance = m_iMaxInstancesCount;

	D3D11_MAPPED_SUBRESOURCE mapSubResource{};
	pContext->Map(m_pInstanceBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&mapSubResource);
	memcpy_s(mapSubResource.pData, sizeof(VTX_INSTANCE_POINT) * numInstance, instanceData, sizeof(VTX_INSTANCE_POINT) * numInstance);
	pContext->Unmap(m_pInstanceBuffer,0);
}
