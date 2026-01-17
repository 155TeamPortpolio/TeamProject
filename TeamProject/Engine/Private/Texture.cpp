#include "Engine_Defines.h"
#include "Texture.h"
#include "Helper_Func.h"
#include "Shader.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CTexture::CTexture()
{
}
static std::wstring ToLowerExt(const std::filesystem::path& pathValue)
{
    std::wstring ext = pathValue.extension().wstring();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    return ext;
}

HRESULT CTexture::Initialize(ID3D11Device* pDevice, const _tchar* filePath, _bool sRGBType)
{
    if (!pDevice || !filePath)
        return E_INVALIDARG;

    Safe_Release(m_pShaderResourceView);
    Safe_Release(m_pResource);

    std::filesystem::path inputPath(filePath);
    inputPath = inputPath.lexically_normal();

    const std::wstring extensionLower = ToLowerExt(inputPath);
    const bool isDDS = (extensionLower == L".dds");
    const bool isTGA = (extensionLower == L".tga");

    if (isTGA)
        return E_FAIL;

    auto TryLoad = [&](const std::filesystem::path& candidatePath) -> HRESULT
        {
            std::error_code errorCode;
            const bool exists = std::filesystem::exists(candidatePath, errorCode);

            if (errorCode)
                return HRESULT_FROM_WIN32(errorCode.value());

            if (!exists)
                return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

            const wchar_t* resolvedPath = candidatePath.c_str();

            if (isDDS)
            {
                auto ddsFlag = sRGBType ? DDS_LOADER_FORCE_SRGB : DDS_LOADER_DEFAULT;
                return CreateDDSTextureFromFileEx(
                    pDevice,
                    resolvedPath,
                    0,
                    D3D11_USAGE_DEFAULT,
                    D3D11_BIND_SHADER_RESOURCE,
                    0, 0,
                    ddsFlag,
                    &m_pResource,
                    &m_pShaderResourceView
                );
            }

            auto wicFlag = sRGBType ? WIC_LOADER_FORCE_SRGB : WIC_LOADER_DEFAULT;
            return CreateWICTextureFromFileEx(
                pDevice,
                resolvedPath,
                0,
                D3D11_USAGE_DEFAULT,
                D3D11_BIND_SHADER_RESOURCE,
                0, 0,
                wicFlag,
                &m_pResource,
                &m_pShaderResourceView
            );
        };

    HRESULT hr = TryLoad(inputPath);
    if (SUCCEEDED(hr))
    {
        Get_TextureDesc(m_Desc);
        return hr;
    }

    filesystem::path absolutePath = std::filesystem::absolute(inputPath).lexically_normal();
    hr = TryLoad(absolutePath);

    if (SUCCEEDED(hr))
    {
        Get_TextureDesc(m_Desc);
        return hr;
    }

    return hr;
}

void CTexture::Render_GUI(_float Width)
{
	if (!m_pShaderResourceView) return;
		
	ImGui::Image((ImTextureID)m_pShaderResourceView, ImVec2(Width-30, Width - 30)); ImGui::SameLine();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image((ImTextureID)m_pShaderResourceView, ImVec2(256, 256));
		ImGui::Text(m_TextureKey.c_str());
		ImGui::EndTooltip();
	}
	ImGui::Text(m_TextureKey.c_str());
}

_bool CTexture::AlphaCheck(TestLevel level)
{
    if (m_AlphaDesc.eLevel >= level)
        return m_AlphaDesc.hasNonOpaque;

    _uint mipIndex = ChooseMipForAlpha(level);
    _uint mipWidth = 0, mipHeight = 0;
    GetMipSize(mipIndex, mipWidth, mipHeight);

    _uint groupX = 0, groupY = 0;
    CalcDispatchGroups(mipWidth, mipHeight, groupX, groupY);

    auto pContext = GameInstance()->Get_Context();
    auto pDevice = GameInstance()->Get_Device();

    auto CSInspector = ResourceManager()->Load_ComputeShader(G_GlobalLevelKey, "CS_AlphaInspect.hlsl");
    if (!CSInspector) return false;

    if (!m_AlphaGlobal.resultUav)
    {
        if (FAILED(CreateAlphaGlobalBuffers(pDevice, m_AlphaGlobal)))
            return false;
    }

    // CB 보장 (중요)
    if (!m_pAlphaInspectCB)
    {
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = sizeof(CB_ALPHA_INSPECT);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(pDevice->CreateBuffer(&cbDesc, nullptr, &m_pAlphaInspectCB)))
            return false;
    }

    CB_ALPHA_INSPECT cb = {};
    cb.textureSize = { mipWidth, mipHeight };
    cb.mipIndex = mipIndex;
    cb.epsilon = 1.0f / 255.0f;

    pContext->UpdateSubresource(m_pAlphaInspectCB, 0, nullptr, &cb, 0, 0);

    CSInspector->Bind(pContext);
    ClearAlphaGlobal(pContext, m_AlphaGlobal.resultUav);

    CSInspector->SetSRV(pContext, 0, m_pShaderResourceView);
    CSInspector->SetUAV(pContext, 0, m_AlphaGlobal.resultUav, 0);
    CSInspector->SetCB(pContext, 0, m_pAlphaInspectCB);

    CSInspector->Dispatch(pContext, groupX, groupY, 1);
    CSInspector->UnbindAll(pContext, 1, 1);

    pContext->CopyResource(m_AlphaGlobal.stagingBuffer, m_AlphaGlobal.resultBuffer);

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (SUCCEEDED(pContext->Map(m_AlphaGlobal.stagingBuffer, 0, D3D11_MAP_READ, 0, &mapped)))
    {
        const _uint flag = *reinterpret_cast<const _uint*>(mapped.pData);
        pContext->Unmap(m_AlphaGlobal.stagingBuffer, 0);

        const _bool hasNonOpaque = (flag != 0);
        m_AlphaDesc.hasNonOpaque = hasNonOpaque;
        m_AlphaDesc.eLevel = level;
        m_AlphaDesc.AlphaAverage = 1.0f; /*현재 반영 안됨*/
    }
    return m_AlphaDesc.hasNonOpaque;
}

_uint CTexture::ChooseMipForAlpha(TestLevel level)
{
    const _uint mipCount = m_Desc.MipLevels;
    if (mipCount == 0)
        return 0;

    const _uint lastMipIndex = mipCount - 1;

    if (level == TestLevel::Precise)
        return 0;

    if (level == TestLevel::Fast)
        return lastMipIndex;

    const uint64_t pixelCount = uint64_t(m_TextureSize.x) * uint64_t(m_TextureSize.y);
    const uint64_t thresholdPixels = uint64_t(2048) * uint64_t(2048);

    if (pixelCount >= thresholdPixels)
        return lastMipIndex;

    return 0;
}

void CTexture::GetMipSize(_uint mipIndex, _uint& outWidth, _uint& outHeight)
{
    _uint baseWidth = m_Desc.Width;
    _uint baseHeight = m_Desc.Height;

    _uint w = baseWidth >> mipIndex; //2배씩 감쇠시키는거 비트 밀어서
    _uint h = baseHeight >> mipIndex;

    if (w == 0) w = 1;
    if (h == 0) h = 1;

    outWidth = w;
    outHeight = h;
}

void CTexture::CalcDispatchGroups(_uint mipWidth, _uint mipHeight, _uint& outGroupX, _uint& outGroupY)
{
    const _uint groupSizeX = 8;
    const _uint groupSizeY = 8;

    outGroupX = (mipWidth + groupSizeX - 1) / groupSizeX;
    outGroupY = (mipHeight + groupSizeY - 1) / groupSizeY;
}

HRESULT CTexture::CreateAlphaGlobalBuffers(ID3D11Device* device, AlphaGlobalGPU& outGpu)
{
    const _uint elementCount = 1;
    const _uint stride = sizeof(_uint);
    const _uint byteWidth = elementCount * stride;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = byteWidth;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = stride;

    HRESULT hr = device->CreateBuffer(&desc, nullptr, &outGpu.resultBuffer);
    if (FAILED(hr)) return hr;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = elementCount;

    hr = device->CreateUnorderedAccessView(outGpu.resultBuffer, &uavDesc, &outGpu.resultUav);
    if (FAILED(hr)) return hr;

    D3D11_BUFFER_DESC staging = desc;
    staging.Usage = D3D11_USAGE_STAGING;
    staging.BindFlags = 0;
    staging.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    staging.MiscFlags = 0;
    staging.StructureByteStride = 0;

    hr = device->CreateBuffer(&staging, nullptr, &outGpu.stagingBuffer);

    if (!m_pAlphaInspectCB)
    {
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = sizeof(CB_ALPHA_INSPECT);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        device->CreateBuffer(&cbDesc, nullptr, &m_pAlphaInspectCB);
    }
    return hr;
}

void CTexture::ClearAlphaGlobal(ID3D11DeviceContext* context, ID3D11UnorderedAccessView* uav)
{
    _uint clearValue[4] = { 0u, 0u, 0u, 0u };
    context->ClearUnorderedAccessViewUint(uav, clearValue);
}

_bool CTexture::Get_TextureDesc(D3D11_TEXTURE2D_DESC& outDesc)
{
    if (!m_pResource)
        return false;
    Safe_Release(m_pTexture);

    ID3D11Texture2D* texture2d = nullptr;
    HRESULT result = m_pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2d);

    if (FAILED(result) || !texture2d)
        return false;

    texture2d->GetDesc(&outDesc);
    m_pTexture = texture2d;
    m_TextureSize = { outDesc.Width, outDesc.Height };
    return true;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, const wstring& filePath, const string& textureKey,_bool sRGBType)
{
	CTexture* instance = new CTexture;

	if (FAILED(instance->Initialize(pDevice, filePath.c_str(), sRGBType))) {
		Safe_Release(instance);
        //string err = "Texture Create Failed : CTexture -" + textureKey;
        //MessageBoxA(nullptr, err.c_str(), "TextureFail", MB_OK);
	}
	else {
		instance->m_TextureKey = textureKey;
	}

	return instance;
}

void CTexture::Free()
{
	__super::Free();
	Safe_Release(m_pShaderResourceView);
	Safe_Release(m_pResource);
	Safe_Release(m_pTexture);
    m_AlphaGlobal.Release();
    Safe_Release(m_pAlphaInspectCB);
}
