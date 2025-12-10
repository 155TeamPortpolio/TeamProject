#include "FontSystem.h"
#include "CustomFont.h"

CFontSystem::CFontSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFontSystem::Initialize()
{
	m_pBatch = new SpriteBatch(m_pContext);
	if (!m_pBatch)
		return E_FAIL;
	m_pStates = new CommonStates(m_pDevice);
	if(!m_pStates)
		return E_FAIL;
	return S_OK;
}
//MakeSpriteFont "Asia신디나루M" "Sindy.spritefont" /FontSize:20 /CharacterRegion:0xAC00-0xD7A3 /FastPack
//
//MakeSpriteFont.exe" "Asia신디나루M" "Sindy.spritefont" ^
/// FontSize:20 ^
/// CharacterRegion : 0x0020 - 0x007E ^
/// CharacterRegion : 0xAC00 - 0xD7A3 ^
/// CharacterRegion : 0x3000 - 0x303F ^
/// CharacterRegion : 0x3130 - 0x318F ^
/// CharacterRegion : 0xFF00 - 0xFFEF ^
/// CharacterRegion : 0x2026 - 0x2026 ^
/// FastPack


HRESULT CFontSystem::Add_Font(string FontName, const wstring& FontFilePath)
{
	auto iter = m_FontsIndex.find(FontName);
	if (iter == m_FontsIndex.end()) {
		CCustomFont* pFont = CCustomFont::Create(m_pDevice, FontFilePath);
		if (pFont) {
			m_Fonts.push_back(pFont);
			m_FontsIndex.emplace(FontName, m_Fonts.size() - 1);
			return S_OK;
		}
	}

	return E_FAIL;
}

CCustomFont* CFontSystem::Get_Font(string FontName)
{
	auto iter = m_FontsIndex.find(FontName);
	if (iter == m_FontsIndex.end()) {
		return nullptr;
	}
	return m_Fonts[iter->second];
}

HRESULT CFontSystem::Push_Text(TEXT_INFO info)
{
	TextData data;
	data.info = info;
	auto iter= m_FontsIndex.find(info.FontTag);
	if (iter == m_FontsIndex.end())
		return E_FAIL;

	data.systemIndex = iter->second;
	m_Texts.push_back(data);
	return S_OK;
}

HRESULT CFontSystem::Render_Font()
{
	sort(m_Texts.begin(), m_Texts.end(), [](const TextData& a, const TextData& b) {
		return a.systemIndex < b.systemIndex;
		});

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	CCustomFont* pFont = nullptr;
	for (size_t i = 0; i < m_Texts.size(); i++)
	{
		if (pFont != m_Fonts[m_Texts[i].systemIndex])
		{
			if (pFont) m_pBatch->End();
			pFont = m_Fonts[m_Texts[i].systemIndex];

#ifdef _DEBUG
			m_pBatch->Begin(
				SpriteSortMode_Deferred,
				m_pStates->Opaque(),   // ← default는 non-premultiplied
				nullptr, nullptr, nullptr, nullptr, XMMatrixIdentity()
			);
#endif // _DEBUG
#ifndef _DEBUG
			m_pBatch->Begin(
				SpriteSortMode_Deferred,
				m_pStates->AlphaBlend(),   // ← default는 non-premultiplied
				nullptr, nullptr, nullptr, nullptr, XMMatrixIdentity()
			);
#endif // _DEBUG
		}
		if (m_Texts[i].info.OutLined) {
			pFont->DrawOutlinedText(m_pBatch,
				m_Texts[i].info.Text,
				m_Texts[i].info.TextPos,
				XMLoadFloat4(&m_Texts[i].info.OutLineColor),
				XMLoadFloat4(&m_Texts[i].info.TextColor),
				m_Texts[i].info.Thickness,
				m_Texts[i].info.Rotation,
				m_Texts[i].info.Origin,
				m_Texts[i].info.Scale
			);
		}
		else {
			pFont->Draw(m_pBatch,
				m_Texts[i].info.Text,
				m_Texts[i].info.TextPos,
				XMLoadFloat4(&m_Texts[i].info.TextColor),
				m_Texts[i].info.Rotation,
				m_Texts[i].info.Origin,
				m_Texts[i].info.Scale
			);
		}
	}

	if (pFont) 
		m_pBatch->End();

	m_Texts.clear(); 
	return S_OK;
}

CFontSystem* CFontSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFontSystem* Instance = new CFontSystem(pDevice, pContext);
	if (FAILED(Instance->Initialize())) {
		Safe_Release(Instance);
	}
	return Instance;
}

void CFontSystem::Free()
{
	__super::Free();
	for (auto& font : m_Fonts)
	{
		Safe_Release(font);
	}
	m_Fonts.clear();
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pStates);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
