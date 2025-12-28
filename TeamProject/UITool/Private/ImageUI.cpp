#include "pch.h"
#include "ImageUI.h"

#include "Sprite2D.h"
#include "UITool_Level.h"
#include "Engine_Math.h"

_uint CImageUI::m_iCount = {};
const string CImageUI::m_strTypeTag = "Image";

CImageUI::CImageUI()
{
}

CImageUI::CImageUI(const CImageUI& rhs)
    : CUIObject_Tool(rhs)
{
}

HRESULT CImageUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CImageUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    if (szTextureKeys.size())
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_iCount++;

    return S_OK;
}

void CImageUI::Priority_Update(_float dt)
{
}

void CImageUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);
}

void CImageUI::Late_Update(_float dt)
{
}

void CImageUI::Render_GUI()
{
    __super::Render_GUI();

    // 텍스쳐
    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    ImGui::SeparatorText(u8"이미지");
    ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 0), ImVec2(300.f, 200.f));
    if (ImGui::Combo(u8"이미지##메인", &m_iTextureKeyIndex, szTextureKeys.data(), szTextureKeys.size()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);
}

void CImageUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    data.strTextureTag = szTextureKeys[m_iTextureKeyIndex];
}

void CImageUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    m_iTextureKeyIndex = Find_TextureIndex(szTextureKeys, data.strTextureTag);
    if (-1 != m_iTextureKeyIndex)
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);
}

CGameObject* CImageUI::Create()
{
    CImageUI* pInstance = new CImageUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CImageUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CImageUI::Clone(INIT_DESC* pArg)
{
    CImageUI* pInstance = new CImageUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CImageUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImageUI::Free()
{
    __super::Free();
}