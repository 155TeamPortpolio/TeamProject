#include "pch.h"
#include "UIObject_Tool.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

CUIObject_Tool::CUIObject_Tool()
{
}

CUIObject_Tool::CUIObject_Tool(const CUIObject_Tool& rhs)
    : CUI_Object(rhs)
{
}

void CUIObject_Tool::LinkChildFromJson(const json& data)
{
    if (!Get_Component<CObjectContainer>())
        return;

    IUI_Service* pUIMgr = CGameInstance::GetInstance()->Get_UIMgr();
    vector<CUI_Object*> UIObjects = pUIMgr->Get_LevelUI(data["levelTag"]);

    for (_int i = 0; i < data["children"].size(); ++i)
    {
        string strChildInstanceKey = data["children"][i];
         
        for (auto& pUIObject : UIObjects)
        {
            if (strChildInstanceKey == pUIObject->Get_InstanceName())
            {
                Get_Component<CObjectContainer>()->Add_Child(pUIObject);
                break;
            }
        }
    }
}

void CUIObject_Tool::ToJson_Common(json& data)
{ 
    // 기본 정보
    data["levelTag"] = m_Level;
    data["instanceKey"] = m_InstanceName;

    // Transform 정보
    data["transform"]["anchorOffset"]["x"] = m_vAnchorOffset.x;
    data["transform"]["anchorOffset"]["y"] = m_vAnchorOffset.y;
    data["transform"]["anchor"] = static_cast<int>(m_eAnchor);
    data["transform"]["size"]["x"] = m_vSize.x;
    data["transform"]["size"]["y"] = m_vSize.y;
    data["transform"]["scale"]["x"] = m_vScale.x;
    data["transform"]["scale"]["y"] = m_vScale.y;
    data["transform"]["pivot"]["x"] = m_vPivot.x;
    data["transform"]["pivot"]["y"] = m_vPivot.y;
    data["transform"]["rotation"] = m_fRadian;

    // 자식들 있다면 
    if (Get_Component<CObjectContainer>())
    {
        data["children"] = json::array();
        for (auto& child : Get_Component<CObjectContainer>()->Get_Children())  // 자식 리스트가 있다면
        {
            data["children"].push_back(child->Get_InstanceName());
        }
    } 
}

void CUIObject_Tool::Render_GUI_Layout()
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

    ImGui::DragFloat2(u8"크기", reinterpret_cast<_float*>(&m_vSize), 1.f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

void CUIObject_Tool::Render_GUI_Transform()
{ 
    ImGui::SeparatorText("Transform");

    ImGui::DragFloat2("Scale", reinterpret_cast<_float*>(&m_vScale), 0.01f, 0.f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

    _float fDegree = XMConvertToDegrees(m_fRadian);
    if(ImGui::DragFloat("Angle", &fDegree, 1.f, -180.f, 180.f))
        m_fRadian = XMConvertToRadians(fDegree);

    _float2 tmpPivot = m_vPivot;
    if (ImGui::DragFloat2("Pivot", reinterpret_cast<_float*>(&tmpPivot), 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        Set_Pivot(tmpPivot);

    ImGui::TextDisabled("LeftTop : %.1f, %.1f", m_vLeftTop.x, m_vLeftTop.y);

    ImGui::TextDisabled("WinSize : %.1f x %.1f", m_WinSize.x, m_WinSize.y);
}

void CUIObject_Tool::Free()
{
    __super::Free();
}