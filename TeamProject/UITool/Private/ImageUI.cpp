#include "pch.h"
#include "ImageUI.h"

#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CImageUI::m_iCount = {};

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
    Render_GUI_Layout();

    Render_GUI_Transform();

    ImGui::SeparatorText(u8"이미지");
    const auto& TextureKeys = CUITool_Level::Get_TextureKeys();
    if(ImGui::Combo(u8"이미지", &m_iTextureKeyIndex, TextureKeys.data(), CUITool_Level::Get_TextureKeysSize()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, TextureKeys[m_iTextureKeyIndex]);
}

json CImageUI::ToJson()
{
    json objData;
     
    objData["typeTag"] = "ImageUI";
    objData["textureTag"] = CUITool_Level::Get_TextureKeys()[m_iTextureKeyIndex];

    ToJson_Common(objData);
    
    return objData;
}

void CImageUI::FromJson(const json& data)
{
    m_eAnchor = static_cast<ANCHOR>(data["transform"]["anchor"].get<int>());     // 앵커는 빌더에 넣기
    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, data["textureTag"]);
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