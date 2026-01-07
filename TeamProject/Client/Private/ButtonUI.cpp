#include "pch.h"
#include "ButtonUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

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

    return S_OK;
}

void CButtonUI::Awake()
{
    __super::Awake();

    Set_Clickable(true);
}

void CButtonUI::Enter_Hover()
{
    if (STATE::DISABLED == m_eState) return;

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
    if (STATE::DISABLED == m_eState) return;

    OutputDebugString(L"Clicked\n");
    m_eState = STATE::CLICKED;

    BTN_EVENT event = {};
    event.msg = Helper::ConvertToWideString(m_szEventMsg);
    CGameInstance::GetInstance()->Get_EventSystem()->Broadcast<BTN_EVENT>({ event });
}

void CButtonUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));
    strcpy_s(m_szEventMsg, data.value("eventMsgTag", "").c_str());
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