#include "pch.h"
#include "TextUI.h"

#include "Sprite2D.h"
#include "TextSlot.h"
#include "UITool_Level.h"
#include "Helper_Func.h"

_uint CTextUI::m_iCount = {};

CTextUI::CTextUI()
{
}

CTextUI::CTextUI(const CTextUI& rhs)
    : CUIObject_Tool(rhs)
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

    const auto& strFontKeys = CUITool_Level::m_strFontKeys;
    if (strFontKeys.size())
        Set_Font(strFontKeys[m_iFontKeyIndex]);
    else
        MSG_BOX("Failed to Set_Font : No Fonts Loaded");

    strcpy_s(m_szText, sizeof(m_szText), u8"텍스트");
    Get_Component<CTextSlot>()->Set_Text(Helper::ConvertToWideString(m_szText));

    Get_Component<CTextSlot>()->Set_Color(m_vFontColor);

#ifdef _DEBUG
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "PanelBox.dds");
#endif

    m_iCount++;

    return S_OK;
}

void CTextUI::Priority_Update(_float dt)
{
}

void CTextUI::Update(_float dt)
{
    Get_Component<CTextSlot>()->Push_Text();

    const float width = Get_Component<CTextSlot>()->Get_TextSize().x;
    switch (m_iAlign)
    {
    case 0: m_vAnchorOffset.x = 0.f;            break;
    case 1: m_vAnchorOffset.x = -width * 0.5f;  break;
    case 2: m_vAnchorOffset.x = -width;         break;
    }
}

void CTextUI::Late_Update(_float dt)
{
}

void CTextUI::Render_GUI()
{
    Render_GUI_Layout();

    Render_GUI_Transform();
    
    ImGui::SeparatorText(u8"콘텐츠");

    if(ImGui::InputTextMultiline(u8"텍스트", (char*)m_szText, sizeof(m_szText), ImVec2(ImGui::GetContentRegionAvail().x, 50.f)))
        Get_Component<CTextSlot>()->Set_Text(Helper::ConvertToWideString(m_szText));
    
    ImGui::SeparatorText(u8"폰트");
    const auto& szFontKeys = CUITool_Level::m_szFontKeys;
    if (ImGui::Combo(u8"폰트", &m_iFontKeyIndex, szFontKeys.data(), szFontKeys.size()))
        Set_Font(szFontKeys[m_iFontKeyIndex]);
    
    if (ImGui::Button(u8"크기 +"))
    {
        m_fFontScale = min(m_fFontScale + 0.1f, 2.f);
        Get_Component<CTextSlot>()->Set_Size(m_fFontScale);
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"크기 -"))
    {
        m_fFontScale = max(m_fFontScale - 0.1f, 0.1f);
        Get_Component<CTextSlot>()->Set_Size(m_fFontScale);
    }

    if(ImGui::ColorPicker4(u8"폰트 컬러", reinterpret_cast<_float*>(&m_vFontColor)))
        Get_Component<CTextSlot>()->Set_Color(m_vFontColor);

    ImGui::SeparatorText(u8"외곽선");
    _bool isOutlined = {};
    if (ImGui::Checkbox(u8"외곽선", &m_isOutlined)) isOutlined = true;
    if(ImGui::DragFloat(u8"굵기", &m_fOutlineThickness, 0.1f, 0.f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) isOutlined = true;
    if(ImGui::ColorPicker4(u8"외곽선 컬러", reinterpret_cast<_float*>(& m_vOutlineColor))) isOutlined = true;

    if (m_isOutlined)
        Get_Component<CTextSlot>()->Set_OutLine(m_fOutlineThickness, m_vOutlineColor);
    else
        Get_Component<CTextSlot>()->ReSet_OutLine();
}

void CTextUI::ToJson(json& data)
{
    __super::ToJson(data);

    data["typeTag"] = "TextUI";

    data["fontTag"] = m_strFontTag;
    data["text"] = m_szText;
    data["fontScale"] = m_fFontScale;
    data["fontColor"]["x"] = m_vFontColor.x;
    data["fontColor"]["y"] = m_vFontColor.y;
    data["fontColor"]["z"] = m_vFontColor.z;
    data["fontColor"]["w"] = m_vFontColor.w;
    data["outlined"] = m_isOutlined;
    data["outlineThickness"] = m_fOutlineThickness;
    data["outlineColor"]["x"] = m_vOutlineColor.x;
    data["outlineColor"]["y"] = m_vOutlineColor.y;
    data["outlineColor"]["z"] = m_vOutlineColor.z;
    data["outlineColor"]["w"] = m_vOutlineColor.w;
}

void CTextUI::FromJson(const json& data)
{
    string strText = data["text"]; 
    strcpy_s(m_szText, sizeof(m_szText), strText.c_str());
    Get_Component<CTextSlot>()->Set_Text(Helper::ConvertToWideString(strText));
    Set_Font(data["fontTag"]);
    m_fFontScale = data["fontScale"];
    Get_Component<CTextSlot>()->Set_Size(m_fFontScale);
    m_vFontColor = _float4(data["fontColor"]["x"].get<float>(), data["fontColor"]["y"].get<float>(), data["fontColor"]["z"].get<float>(), data["fontColor"]["w"].get<float>());
    Get_Component<CTextSlot>()->Set_Color(m_vFontColor);

    Update_UITransform();
    Get_Component<CTextSlot>()->Set_Position(m_vLeftTop);

    m_isOutlined = data["outlined"];
    m_fOutlineThickness = data["outlineThickness"];
    m_vOutlineColor = _float4(data["outlineColor"]["x"].get<_float>(), data["outlineColor"]["y"].get<_float>(), data["outlineColor"]["z"].get<_float>(), data["outlineColor"]["w"].get<_float>());
    if(m_isOutlined)
        Get_Component<CTextSlot>()->Set_OutLine(m_fOutlineThickness, m_vOutlineColor);

    __super::FromJson(data);
    FromJson_RefreshCount(m_iCount);    // json에서 불러올 때 카운트 새로고침
}

void CTextUI::Render_GUI_Layout()
{
    ImGui::SeparatorText("Layout");

    ImGui::Text("Anchor");
    struct AnchorPreset { const char* label; ANCHOR value; };
    static const AnchorPreset presets[9] = {
        {"##TL", (ANCHOR)((_uint)ANCHOR::Top | (_uint)ANCHOR::Left)},
        {"##TC", (ANCHOR)((_uint)ANCHOR::Top)},
        {"##TR", (ANCHOR)((_uint)ANCHOR::Top | (_uint)ANCHOR::Right)},

        {"##CL", (ANCHOR)((_uint)ANCHOR::Left)},
        {"##CC", (ANCHOR)((_uint)ANCHOR::Center)},
        {"##CR", (ANCHOR)((_uint)ANCHOR::Right)},

        {"##BL", (ANCHOR)((_uint)ANCHOR::Bottom | (_uint)ANCHOR::Left)},
        {"##BC", (ANCHOR)((_uint)ANCHOR::Bottom)},
        {"##BR", (ANCHOR)((_uint)ANCHOR::Bottom | (_uint)ANCHOR::Right)},
    };

    auto IsSelected = [&](ANCHOR a) { return (_uint)a == (_uint)m_eAnchor; };

    float cell = ImGui::GetFrameHeight(); // 정사각형 느낌
    ImVec2 btnSize(cell * 1.2f, cell * 1.2f);

    for (int i = 0; i < 9; ++i)
    {
        if (i % 3 != 0) ImGui::SameLine();

        bool selected = IsSelected(presets[i].value);
        if (selected) ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        else          ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

        if (selected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

        if (ImGui::Button(presets[i].label, btnSize))
            m_eAnchor = presets[i].value;

        if (selected) ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::TextDisabled("Selected: %u", (_uint)m_eAnchor);

    ImGui::DragFloat2(u8"위치", reinterpret_cast<_float*>(&m_vAnchorOffset));
    Get_Component<CTextSlot>()->Set_Position(m_vLeftTop);

    if (ImGui::BeginTable("TextAlign", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_PadOuterX))
    {
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text(u8"정렬");

        ImGui::TableSetColumnIndex(1);

        bool changed = false;

        changed |= ImGui::RadioButton(u8"왼쪽", &m_iAlign, 2);
        ImGui::SameLine();
        changed |= ImGui::RadioButton(u8"가운데", &m_iAlign, 1);
        ImGui::SameLine();
        changed |= ImGui::RadioButton(u8"오른쪽", &m_iAlign, 0);

        if (changed)
        {
            const float width = Get_Component<CTextSlot>()->Get_TextSize().x;

            switch (m_iAlign)
            {
            case 0: m_vAnchorOffset.x = 0.f;            break;
            case 1: m_vAnchorOffset.x = -width * 0.5f;  break;
            case 2: m_vAnchorOffset.x = -width;         break;
            }
        }

        ImGui::EndTable();
    }

    ImGui::Checkbox("Size to Content", &m_isSizeToContent);

    if (m_isSizeToContent)
        m_vSize = Get_Component<CTextSlot>()->Get_TextSize() * m_fFontScale;
    else
        ImGui::DragFloat2(u8"크기", reinterpret_cast<_float*>(&m_vSize), 1.f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

void CTextUI::Render_GUI_Transform()
{
    ImGui::SeparatorText("Transform");

    ImGui::DragFloat2("Scale", reinterpret_cast<_float*>(&m_vScale), 0.01f, 0.f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

    _float fDegree = XMConvertToDegrees(m_fRadian);
    if (ImGui::DragFloat("Angle", &fDegree, 1.f, -180.f, 180.f))
    { 
        m_fRadian = XMConvertToRadians(fDegree);
        Get_Component<CTextSlot>()->Set_Rotate(m_fRadian);
    } 

    _float2 tmpPivot = m_vPivot;
    if (ImGui::DragFloat2("Pivot", reinterpret_cast<_float*>(&tmpPivot), 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
    {
        Set_Pivot(tmpPivot);
        //Get_Component<CTextSlot>()->Set_Origin(tmpPivot);
    } 

    ImGui::TextDisabled("LeftTop : %.1f, %.1f", m_vLeftTop.x, m_vLeftTop.y);

    ImGui::TextDisabled("WinSize : %.1f x %.1f", m_WinSize.x, m_WinSize.y);
}

void CTextUI::Set_Font(const string& strFontTag)
{
    Get_Component<CTextSlot>()->Set_Font(strFontTag);
    m_strFontTag = strFontTag;
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