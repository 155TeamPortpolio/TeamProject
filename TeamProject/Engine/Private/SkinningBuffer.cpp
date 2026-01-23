#include "Engine_Defines.h"
#include "SkinningBuffer.h"

CSkinningBuffer::CSkinningBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CSkinningBuffer::Initialize()
{
    CreatePages();
}

void CSkinningBuffer::BeginFrame() 
{
    /*초기화*/
    m_CurIndex = 0;
    for (auto& page : m_Pages)
    {
        page.CursorBones = 0;
        page.begunThisFrame = false;
    }
}

_bool CSkinningBuffer::Upload(const void* matrixFloat4x4Array, _uint boneCount, Allocation& outAlloc) 
{

}
void CSkinningBuffer::BindSRV(ID3D11DeviceContext* context, _uint slot) const 
{

}

ID3D11ShaderResourceView* CSkinningBuffer::GetSRV(_uint pageIndex) const 
{

}

_bool CSkinningBuffer::CreatePages()
{
    m_Pages.clear();
    m_Pages.resize(BonePages);

    const _uint matrixByteSize = sizeof(_float4x4);
    const _uint bufferByteSize = matrixByteSize * g_iMaxNumBones;

    for (_uint pageIndex = 0; pageIndex < BonePages; ++pageIndex)
    {
        SkinningPage& page = m_Pages[pageIndex];
        page.Capacity = g_iMaxNumBones;
        page.CursorBones = 0;
        page.begunThisFrame = false;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = bufferByteSize;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = matrixByteSize;

        HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &page.pBuffer);
        if (FAILED(hr) || !page.pBuffer)
            return false;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = g_iMaxNumBones;

        hr = m_pDevice->CreateShaderResourceView(page.pBuffer, &srvDesc, &page.pShaderResourceView);
        if (FAILED(hr) || !page.pShaderResourceView)
            return false;
    }

    return true;
}

_bool CSkinningBuffer::EnsurePageBegun(ID3D11DeviceContext* context, SkinningPage& page, bool discard)
{
    /*열고 닫기만->*/
    if (page.begunThisFrame)
        return true;

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    const D3D11_MAP mapType = discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;

    HRESULT hr = context->Map(page.pBuffer, 0, mapType, 0, &mapped);
    if (FAILED(hr))
        return false;

    context->Unmap(page.pBuffer, 0);
    page.begunThisFrame = true;
    return true;
}

_bool CSkinningBuffer::WriteToPage(ID3D11DeviceContext* context, SkinningPage& page, const void* matrixFloat4x4Array, _uint boneCount, _uint& outOffset)
{
    if (!matrixFloat4x4Array || boneCount == 0)
        return false;

    if (page.CursorBones + boneCount > page.Capacity)
        return false;

    const _uint matrixByteSize = sizeof(_float4x4);
    const _uint writeOffsetBytes = page.CursorBones * matrixByteSize;
    const _uint writeSizeBytes = boneCount * matrixByteSize;

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = context->Map(page.pBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped);
    if (FAILED(hr))
        return false;

    std::uint8_t* dstBytes = static_cast<std::uint8_t*>(mapped.pData) + writeOffsetBytes;
    const std::uint8_t* srcBytes = static_cast<const std::uint8_t*>(matrixFloat4x4Array);

    std::memcpy(dstBytes, srcBytes, writeSizeBytes);

    context->Unmap(page.pBuffer, 0);

    outOffset = page.CursorBones;
    page.CursorBones += boneCount;
    return true;
}

CSkinningBuffer* CSkinningBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkinningBuffer* instance = new CSkinningBuffer(pDevice, pContext);
	if (FAILED(instance->Initialize())) 
	{
		Safe_Release(instance);
	}
	return instance;
}

void CSkinningBuffer::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

    for (auto& page : m_Pages)
        page.Release();
}