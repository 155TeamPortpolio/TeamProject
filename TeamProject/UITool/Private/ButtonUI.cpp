#include "pch.h"
#include "ButtonUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CButtonUI::m_iCount = {};
const string CButtonUI::m_strTypeTag = "Button";

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

    Set_Clickable(true);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    if (szTextureKeys.size())
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_iCount++;

    return S_OK;
}

void CButtonUI::Priority_Update(_float dt)
{
}

void CButtonUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);
}

void CButtonUI::Late_Update(_float dt)
{
}

void CButtonUI::Render_GUI()
{
    Render_GUI_Layout();

    Render_GUI_Transform();

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    // 이미지
    ImGui::SeparatorText(u8"이미지");
    ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 0), ImVec2(300.f, 200.f));
    if (ImGui::Combo(u8"이미지##메인", &m_iTextureKeyIndex, szTextureKeys.data(), szTextureKeys.size()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    ImGui::SeparatorText(u8"이벤트");
    ImGui::InputText(u8"메시지",static_cast<_char*>(m_szEventMsg), sizeof(m_szEventMsg));

    ImGui::SeparatorText(u8"상태");
    string strState = {};
    switch (m_eState)
    { 
    case STATE::NORMAL: strState = ENUM_TO_STRING(STATE::NORMAL); break;
    case STATE::HOVERED: strState = ENUM_TO_STRING(STATE::HOVERED); break;
    case STATE::CLICKED: strState = ENUM_TO_STRING(STATE::CLICKED); break;
    case STATE::DISABLED: strState = ENUM_TO_STRING(STATE::DISABLED); break;
    }
    ImGui::TextDisabled(strState.c_str());

    Render_GUI_TextKey();

    __super::Render_GUI();
}

void CButtonUI::Enter_Hover()
{
    if (STATE::DISABLED == m_eState)
        return;

    OutputDebugString(L"Enter_Hover\n");
    m_eState = STATE::HOVERED;
}

void CButtonUI::Exit_Hover()
{
    OutputDebugString(L"Exit_Hover\n");
    m_eState = STATE::NORMAL;
}

void CButtonUI::OnClick()
{
    if (STATE::DISABLED == m_eState)
        return;

    OutputDebugString(L"Clicked\n");
    m_eState = STATE::CLICKED;

    BTN_EVENT event = {};
    event.msg = Helper::ConvertToWideString(m_szEventMsg);
    CGameInstance::GetInstance()->Get_EventSystem()->Broadcast<BTN_EVENT>({ event });
}

void CButtonUI::ToJson(json& data)
{
    __super::ToJson(data);

    data["typeTag"] = "ButtonUI";

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    data["textureTag"] = szTextureKeys[m_iTextureKeyIndex];

    data["eventMsg"] = m_szEventMsg;
}

void CButtonUI::FromJson(const json& data)
{
    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;

    m_iTextureKeyIndex = Find_TextureIndex(szTextureKeys, data["textureTag"]);
    if (-1 != m_iTextureKeyIndex)
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    strcpy_s(m_szEventMsg, sizeof(m_szEventMsg), data["eventMsg"].get<string>().c_str());

    __super::FromJson(data);
    FromJson_RefreshCount(m_iCount);    // json에서 불러올 때 카운트 새로고침
}

void CButtonUI::SavePrefab(json& data)
{
    __super::SavePrefab(data);

    data["typeTag"] = m_strTypeTag;

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    data["textureTag"] = szTextureKeys[m_iTextureKeyIndex];

    data["eventMsg"] = m_szEventMsg;
}

void CButtonUI::LoadPrefab(const json& data)
{
    __super::LoadPrefab(data);

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    m_iTextureKeyIndex = Find_TextureIndex(szTextureKeys, data["textureTag"]);
    if (-1 != m_iTextureKeyIndex)
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    strcpy_s(m_szEventMsg, sizeof(m_szEventMsg), data["eventMsg"].get<string>().c_str());
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