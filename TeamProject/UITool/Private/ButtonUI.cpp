#include "pch.h"
#include "ButtonUI.h"

#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CButtonUI::m_iCount = {};

CButtonUI::CButtonUI()
{
}

CButtonUI::CButtonUI(const CButtonUI& rhs)
    : CUIObject_Tool(rhs)
{
}

HRESULT CButtonUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CButtonUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    const auto& strTextureKeys = CUITool_Level::m_strTextureKeys;
    for (_int i = 0; i < static_cast<_int>(STATE::END); ++i)
    { 
        if (strTextureKeys.size())
            Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, strTextureKeys[m_iTextureKeyIndices[i]]);
    }

    return S_OK;
}

void CButtonUI::Priority_Update(_float dt)
{
}

void CButtonUI::Update(_float dt)
{
}

void CButtonUI::Late_Update(_float dt)
{
}

void CButtonUI::Render_GUI()
{
    Render_GUI_Layout();

    ImGui::SeparatorText(u8"이미지 넣기");
    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    Render_GUI_Texture(STATE::NORMAL, u8"normal", szTextureKeys, G_GlobalLevelKey);
    Render_GUI_Texture(STATE::CLICKED, u8"clicked", szTextureKeys, G_GlobalLevelKey);
    Render_GUI_Texture(STATE::DISABLED, u8"disabled", szTextureKeys, G_GlobalLevelKey);

    ImGui::SeparatorText(u8"이미지 보기");
    _bool isChanged = {};
    isChanged |= ImGui::RadioButton(u8"show normal", &m_iState, static_cast<_int>(STATE::NORMAL));
    isChanged |= ImGui::RadioButton(u8"show clicked", &m_iState, static_cast<_int>(STATE::CLICKED));
    isChanged |= ImGui::RadioButton(u8"show disabled", &m_iState, static_cast<_int>(STATE::DISABLED));
    if(isChanged)
        Get_Component<CSprite2D>()->ChangeSprite(m_iState);
}

void CButtonUI::ToJson(json& data)
{
    __super::ToJson(data);

    data["typeTag"] = "ImageUI";

    // 추후 추가 예정
    //data["textureTag"] = m_strTextureKey;// CUITool_Level::m_strTextureKeys[m_iTextureKeyIndex];
}

void CButtonUI::FromJson(const json& data)
{
    // 추후 추가 예정
}

void CButtonUI::Render_GUI_Texture(STATE eState, const char* label, const vector<const _char*>& szTextureKeys, const string& levelKey)
{
    _int iState = static_cast<_int>(eState);

    if (ImGui::Combo(label, &m_iTextureKeyIndices[iState], szTextureKeys.data(), szTextureKeys.size()))
        Change_Texture(iState, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndices[iState]], m_strTextureKeys[iState]);
}

CGameObject* CButtonUI::Create()
{
    CButtonUI* pInstance = new CButtonUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButtonUI::Clone(INIT_DESC* pArg)
{
    CButtonUI* pInstance = new CButtonUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButtonUI::Free()
{
    __super::Free();
}