#include "pch.h"
#include "ButtonUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CButtonUI::m_iCount = {};
const string CButtonUI::m_strTypeTag = "Button";

HRESULT CButtonUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CButtonUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_Clickable(true);

    Set_OriginTexSize(true);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_iCount++;

    return S_OK;
}

void CButtonUI::Update(_float dt)
{
    if (!m_isAlive) return;

    Play_Animation(dt);
}

void CButtonUI::Render_GUI()
{
    __super::Render_GUI();

    Render_GUI_Image(m_strTextureKey);

    ImGui::SeparatorText(u8"이벤트");
    ImGui::InputText(u8"메시지", static_cast<_char*>(m_szEventMsg), sizeof(m_szEventMsg));

    ImGui::SeparatorText(u8"버튼 상태");
    string strState = {};
    switch (m_eState)
    {
    case STATE::NORMAL:   strState = ENUM_TO_STRING(STATE::NORMAL);   break;
    case STATE::HOVERED:  strState = ENUM_TO_STRING(STATE::HOVERED);  break;
    case STATE::CLICKED:  strState = ENUM_TO_STRING(STATE::CLICKED);  break;
    case STATE::DISABLED: strState = ENUM_TO_STRING(STATE::DISABLED); break;
    }
    ImGui::TextDisabled(strState.c_str());
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

void CButtonUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["textureTag"] = m_strTextureKey;

    data["eventMsgTag"] = m_szEventMsg;
}

void CButtonUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("textureTag", "");
    strcpy_s(m_szEventMsg, data.value("eventMsgTag", "").c_str());

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);
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