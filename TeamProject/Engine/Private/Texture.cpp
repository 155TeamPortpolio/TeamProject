#include "Texture.h"
#include "Helper_Func.h"
#include "Shader.h"

CTexture::CTexture()
{
}

HRESULT CTexture::Initialize(ID3D11Device* pDevice, const _tchar* filePath)
{
		string extension = filesystem::path(filePath).extension().string();
	
		HRESULT hr;
	
		if (extension == ".dds") {
			hr = CreateDDSTextureFromFile(pDevice, filePath, nullptr, &m_pShaderResourceView);
		}
		else if (extension == ".tga") {
			hr = E_FAIL;
		}
		else {
			hr = CreateWICTextureFromFile(pDevice, filePath, nullptr, &m_pShaderResourceView);
		}
	
		if (FAILED(hr))
			return hr;

	return S_OK;
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

CTexture* CTexture::Create(ID3D11Device* pDevice, const wstring& filePath, const string& textureKey)
{
	CTexture* instance = new CTexture;

	if (FAILED(instance->Initialize(pDevice, filePath.c_str()))) {
		Safe_Release(instance);
		MSG_BOX("Texture Create Failed : CTexture");
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
