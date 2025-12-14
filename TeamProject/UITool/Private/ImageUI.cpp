#include "pch.h"
#include "ImageUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
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
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "Logo.png");

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
    ImGui::SeparatorText("Appearance");
    const auto& TextureKeys = CUITool_Level::Get_TextureKeys();
    if(ImGui::Combo("Image", &m_iTextureKeyIndex, TextureKeys.data(), CUITool_Level::Get_TextureKeysSize()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, TextureKeys[m_iTextureKeyIndex]);

    ImGui::SeparatorText("Layout");
    ImGui::DragFloat("X Position", &m_fLocalX, 1.f);
    ImGui::DragFloat("Y Position", &m_fLocalY, 1.f);
    ImGui::DragFloat("X Size", &m_fSizeX, 1.f);
    ImGui::DragFloat("Y Size", &m_fSizeY, 1.f);

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