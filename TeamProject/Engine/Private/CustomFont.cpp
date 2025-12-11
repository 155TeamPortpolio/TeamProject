#include "Engine_Defines.h"
#include "CustomFont.h"
#include "GameInstance.h"
CCustomFont::CCustomFont()
{
}

HRESULT CCustomFont::Initialize(ID3D11Device* pDevice, const wstring& FontFilePath)
{
    m_pFont = new SpriteFont(pDevice, FontFilePath.c_str());
    if (!m_pFont)
        return E_FAIL;

    m_pBatch = new SpriteBatch(CGameInstance::GetInstance()->Get_Context());
    if (!m_pBatch)
        return E_FAIL;

    m_pStates = new CommonStates(pDevice);
    if (!m_pStates)
        return E_FAIL;

    return S_OK;
}

void CCustomFont::Draw(SpriteBatch* pBatch,wstring pText,const _float2& vPosition, _fvector vColor,
    _float rotation,const _float2& origin,_float scale)
{
    if (!m_pFont) return; // 폰트 로드 실패 시 안전하게 리턴
    m_pFont->DrawString(
        pBatch, pText.c_str(), vPosition, vColor,
        rotation,    origin,  scale      
    );

    //m_pFont->MeasureString()
}

void CCustomFont::DrawOutlinedText(
    SpriteBatch* pBatch, wstring text, const _float2& vPosition, _fvector outlineColor, _fvector textColor, _float thickness, _float rotation, const _float2& origin, _float scale)
{
    static const XMFLOAT2 offsets[] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},           {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };

    for (auto& o : offsets)
    {
        XMFLOAT2 off = {
              vPosition.x + o.x * thickness * scale,
              vPosition.y + o.y * thickness * scale
        };
        m_pFont->DrawString(pBatch, text.c_str(), off, outlineColor, rotation, origin, scale);
    }

    // 본문
    m_pFont->DrawString(
        pBatch, text.c_str(), vPosition, textColor,
        rotation, origin, scale
    );
}

void CCustomFont::DrawSelf(wstring pText, const _float2& vPosition, _fvector vColor, _float rotation, const _float2& origin, _float scale)
{
    m_pBatch->Begin(
        SpriteSortMode_Deferred,
        m_pStates->AlphaBlend(),   // ← default는 non-premultiplied
        nullptr, nullptr, nullptr, nullptr, XMMatrixIdentity()
    );

    m_pFont->DrawString(m_pBatch,
        pText.c_str(),
        vPosition,
        vColor,
        rotation,
        origin,
        scale
    );

    m_pBatch->End();
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, const wstring& FontFilePath)
{
    CCustomFont* pInstance = new CCustomFont();

    if (FAILED(pInstance->Initialize(pDevice,  FontFilePath)))
    {
        MSG_BOX("Failed to Created : CCustomFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCustomFont::Free()
{
    __super::Free();
    Safe_Delete(m_pFont);
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pStates);
}
