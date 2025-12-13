#include "pch.h"
#include "TextUI.h"

#include "TextSlot.h"
#include "UITool_Level.h"

CTextUI::CTextUI()
{
}

CTextUI::CTextUI(const CTextUI& rhs)
    : CUI_Object(rhs)
{
}

HRESULT CTextUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CTextSlot>();

    return S_OK;
}

HRESULT CTextUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CTextSlot>()->Set_Font(CUITool_Level::Get_FontKeys()[m_iFontKeyIndex]);
    Get_Component<CTextSlot>()->Set_Text(L"텍스트 블록");
    Get_Component<CTextSlot>()->Set_Color(m_vColor);
    Get_Component<CTextSlot>()->Set_Position({ m_fLocalX, m_fLocalY });

    return S_OK;
}

void CTextUI::Priority_Update(_float dt)
{
}

void CTextUI::Update(_float dt)
{
    Get_Component<CTextSlot>()->Push_Text();
}

void CTextUI::Late_Update(_float dt)
{
}

void CTextUI::Render_GUI()
{
    ImGui::SeparatorText(u8"레이아웃");
    _bool isMoved = {};
    if (ImGui::DragFloat(u8"X 위치", &m_fLocalX, 1.f)) isMoved = true;
    if (ImGui::DragFloat(u8"Y 위치", &m_fLocalY, 1.f)) isMoved = true;
    ImGui::DragFloat(u8"X 크기", &m_fSizeX, 1.f);   // 앵커할 때 폰트 사이즈가 영향이 있을것 같음
    ImGui::DragFloat(u8"Y 크기", &m_fSizeY, 1.f);
    if (isMoved)
        Get_Component<CTextSlot>()->Set_Position({ m_fLocalX, m_fLocalY });

    ImGui::SeparatorText(u8"콘텐츠");
    if(ImGui::InputTextMultiline(u8"텍스트", (char*)m_szText, sizeof(m_szText)))
        Get_Component<CTextSlot>()->Set_Text(Utf8ToWstring(m_szText));

    ImGui::SeparatorText(u8"폰트");
    const auto& FontKeys = CUITool_Level::Get_FontKeys();
    if (ImGui::Combo(u8"폰트", &m_iFontKeyIndex, FontKeys.data(), CUITool_Level::Get_FontKeysSize()))
        Get_Component<CTextSlot>()->Set_Font(FontKeys[m_iFontKeyIndex]);

    if(ImGui::ColorPicker4(u8"컬러", reinterpret_cast<_float*>(&m_vColor)))
        Get_Component<CTextSlot>()->Set_Color(m_vColor);

    CGameObject::Render_GUI();
}

wstring CTextUI::Utf8ToWstring(const string& strUtf8)
{
    if (strUtf8.empty())
        return wstring();

    int iSize = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), (int)strUtf8.size(), NULL, 0);
    wstring strWstr(iSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), (int)strUtf8.size(), &strWstr[0], iSize);

    return strWstr;
}

CGameObject* CTextUI::Create()
{
    CTextUI* pInstance = new CTextUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CTextUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTextUI::Clone(INIT_DESC* pArg)
{
    CTextUI* pInstance = new CTextUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CTextUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTextUI::Free()
{
    __super::Free();
}