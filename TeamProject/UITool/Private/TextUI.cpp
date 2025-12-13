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

    if (FAILED(Get_Component<CTextSlot>()->Set_Font("NotoSansKR_Regular_24")))  // 해당 폰트로 설정 실패할 경우 0번째 폰트로
        Get_Component<CTextSlot>()->Set_Font(CUITool_Level::Get_FontKeys()[0]);
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
    ImGui::SeparatorText("Layout");
    _bool isMoved = {}; 
    if (ImGui::DragFloat("X Position", &m_fLocalX, 1.f)) isMoved = true;
    if (ImGui::DragFloat("Y Position", &m_fLocalY, 1.f)) isMoved = true;
    ImGui::DragFloat("X Size", &m_fSizeX, 1.f);   // 앵커할 때 폰트 사이즈가 영향이 있을것 같음
    ImGui::DragFloat("Y Size", &m_fSizeY, 1.f);
    if(isMoved)
        Get_Component<CTextSlot>()->Set_Position({ m_fLocalX, m_fLocalY });

    ImGui::SeparatorText("Font");
    if (ImGui::InputTextMultiline("Text", m_szText, MAX_PATH))
    {
        _wstring strText = _wstring(m_szText, &m_szText[MAX_PATH]);
        Get_Component<CTextSlot>()->Set_Text(strText);
    } 
    const auto& FontKeys = CUITool_Level::Get_FontKeys();
    if (ImGui::Combo("Font", &m_iFontKeyIndex, FontKeys.data(), CUITool_Level::Get_FontKeysSize()))
        Get_Component<CTextSlot>()->Set_Font(FontKeys[m_iFontKeyIndex]);

    if(ImGui::ColorPicker4("Color", reinterpret_cast<_float*>(&m_vColor)))
        Get_Component<CTextSlot>()->Set_Color(m_vColor);

    CGameObject::Render_GUI();
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