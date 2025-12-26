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

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "empty.png");

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
    __super::Render_GUI();
     
    // 텍스쳐
    ImGui::SeparatorText(u8"이미지");
    if (ImGui::Button(u8"선택"))
    {
        string filePath = Helper::OpenFile_Dialogue();
        if (!filePath.empty())
        {
            string fileName = Helper::GetFileNameWithExtension(filePath);

            CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, filePath);
            m_strTextureKey = fileName;
            Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);
        }
    }

    // 이벤트
    ImGui::SeparatorText(u8"이벤트");
    ImGui::InputText(u8"메시지",static_cast<_char*>(m_szEventMsg), sizeof(m_szEventMsg));

    // 상태 표시
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

void CButtonUI::SavePrefab(json& data)
{
    __super::SavePrefab(data);

    data["typeTag"] = m_strTypeTag;

    data["textureTag"] = m_strTextureKey;

    data["eventMsg"] = m_szEventMsg;
}

void CButtonUI::LoadPrefab(const json& data)
{
    __super::LoadPrefab(data);

    m_strTextureKey = data["textureTag"];
    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);

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