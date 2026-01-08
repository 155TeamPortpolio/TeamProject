#include "Engine_Defines.h"
#include "Texture.h"
#include "Helper_Func.h"
#include "Shader.h"

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
        Extract_Size();
        return hr;
    }

    std::filesystem::path absolutePath = std::filesystem::absolute(inputPath).lexically_normal();
    hr = TryLoad(absolutePath);
    if (SUCCEEDED(hr))
    {
        Extract_Size();
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

void CTexture::Extract_Size()
{
    if (!m_pResource)
        return;

    D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    m_pResource->GetType(&dimension);

    if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        return;

    ID3D11Texture2D* texture2D = nullptr;
    HRESULT hrQuery = m_pResource->QueryInterface(
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&texture2D)
    );
    if (FAILED(hrQuery) || !texture2D)
        return;

    D3D11_TEXTURE2D_DESC desc = {};
    texture2D->GetDesc(&desc);

    m_TextureSize.x = desc.Width;
    m_TextureSize.y = desc.Height;

    Safe_Release(texture2D);
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
}
