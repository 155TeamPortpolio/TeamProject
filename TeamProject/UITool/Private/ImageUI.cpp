#include "pch.h"
#include "ImageUI.h"

#include "Sprite2D.h"
#include "UITool_Level.h"

CImageUI::CImageUI()
{
}

CImageUI::CImageUI(const CImageUI& rhs)
    : CUI_Object(rhs)
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
    if (CUITool_Level::Get_TextureKeysSize())
        Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, CUITool_Level::Get_TextureKeys()[m_iTextureKeyIndex]);
    else
        MSG_BOX("Failed to Add_Texture : No Textures Loaded");

    return S_OK;
}

void CImageUI::Priority_Update(_float dt)
{
}

void CImageUI::Update(_float dt)
{
}

void CImageUI::Late_Update(_float dt)
{
}

void CImageUI::Render_GUI()
{
    ImGui::SeparatorText(u8"레이아웃");
    ImGui::DragFloat(u8"X 위치", &m_fLocalX, 1.f);
    ImGui::DragFloat(u8"Y 위치", &m_fLocalY, 1.f);
    ImGui::DragFloat(u8"X 크기", &m_fSizeX, 1.f);
    ImGui::DragFloat(u8"Y 크기", &m_fSizeY, 1.f);

    ImGui::SeparatorText("Appearance");
    const auto& TextureKeys = CUITool_Level::Get_TextureKeys();
    if(ImGui::Combo("Image", &m_iTextureKeyIndex, TextureKeys.data(), CUITool_Level::Get_TextureKeysSize()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, TextureKeys[m_iTextureKeyIndex]);

    // 컬러 추가 필요. 이미지에 multiply

    CGameObject::Render_GUI();
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