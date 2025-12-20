#include "Engine_Defines.h"
#include "StructuredBuffer.h"
#include "GameInstance.h"

CStructuredBuffer::CStructuredBuffer()
{
}

HRESULT CStructuredBuffer::Initialize(const DESC& desc)
{
	if (FAILED(CreateBuffer(desc)))
		return E_FAIL;

	if (desc.UseSRV)
	{
		if (FAILED(CreateSRV()))
			return E_FAIL;
	}

	if (desc.UseUAV)
	{
		if (FAILED(CreateUAV(desc.iUAVFlag)))
			return E_FAIL;
	}

	if (desc.UseStaging)
	{
		if (FAILED(CreateStaging()))
			return E_FAIL;
	}

	return S_OK;
}

CStructuredBuffer* CStructuredBuffer::Create(const DESC& desc)
{
	CStructuredBuffer* instance = new CStructuredBuffer();

	if (FAILED(instance->Initialize(desc)))
	{
		MSG_BOX("Object Clone Failed : CStructuredBuffer");
		Safe_Release(instance);
	}

	return instance;
}

HRESULT CStructuredBuffer::CreateBuffer(const DESC& desc)
{
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	m_iCount = desc.iCount;
	m_iStride = desc.iStride;
	
	D3D11_BUFFER_DESC BufferDesc{};
	BufferDesc.ByteWidth = m_iStride * m_iCount;

	if(desc.UseSRV)
		BufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (desc.UseSRV)
		BufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.StructureByteStride = m_iStride;

	HRESULT hr{};
	if (desc.pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData{};
		InitData.pSysMem = desc.pInitData;

		hr = pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pBuffer);
	}
	else
	{
		hr = pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pBuffer);
	}

	return hr;
}

HRESULT CStructuredBuffer::CreateSRV()
{
	if (!m_pBuffer)
		return E_FAIL;

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_iCount;

	if (FAILED(pDevice->CreateShaderResourceView(m_pBuffer, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CreateUAV(_uint iUAVFlag)
{
	if (!m_pBuffer)
		return E_FAIL;

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_iCount;
	UAVDesc.Buffer.Flags = iUAVFlag;    

	if (FAILED(pDevice->CreateUnorderedAccessView(m_pBuffer, &UAVDesc, &m_pUAV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CreateStaging()
{
	if (!m_pBuffer)
		return E_FAIL;

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	D3D11_BUFFER_DESC BufferDesc{};
	m_pBuffer->GetDesc(&BufferDesc);

	D3D11_BUFFER_DESC bd = BufferDesc;
	bd.BindFlags = 0;
	bd.MiscFlags = 0;                      
	bd.Usage = D3D11_USAGE_STAGING;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if (FAILED(pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pStaging)))
		return E_FAIL;

	return S_OK;
}
