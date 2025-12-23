#include "Engine_Defines.h"
#include "Texture.h"
#include "Helper_Func.h"
#include "Shader.h"

CTexture::CTexture()
{
}
HRESULT CTexture::Initialize(ID3D11Device* pDevice, const _tchar* filePath, _bool sRGBType)
{
    std::string extension = std::filesystem::path(filePath).extension().string();
    HRESULT hr = E_FAIL;

    const bool isDDS = (extension == ".dds");
    const bool isTGA = (extension == ".tga");

    if (isTGA)
        return E_FAIL; // TODO: TGA ·Î´õ

    if (isDDS)
    {
        auto ddsFlag = sRGBType ? DDS_LOADER_FORCE_SRGB : DDS_LOADER_DEFAULT;

        hr = CreateDDSTextureFromFileEx(
            pDevice,
            filePath,
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0, 0,
            ddsFlag,
            nullptr,
            &m_pShaderResourceView
        );
    }
    else
    {
        auto wicFlag = sRGBType ? WIC_LOADER_FORCE_SRGB : WIC_LOADER_DEFAULT;

        hr = CreateWICTextureFromFileEx(
            pDevice,
            filePath,
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0, 0,
            wicFlag,
            nullptr,
            &m_pShaderResourceView
        );
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

CTexture* CTexture::Create(ID3D11Device* pDevice, const wstring& filePath, const string& textureKey,_bool sRGBType)
{
	CTexture* instance = new CTexture;

	if (FAILED(instance->Initialize(pDevice, filePath.c_str(), sRGBType))) {
		Safe_Release(instance);
		//MSG_BOX("Texture Create Failed : CTexture");
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
}
