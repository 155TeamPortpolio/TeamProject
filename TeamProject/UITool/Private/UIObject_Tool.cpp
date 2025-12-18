#include "pch.h"
#include "UIObject_Tool.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

CUIObject_Tool::CUIObject_Tool()
{
}

CUIObject_Tool::CUIObject_Tool(const CUIObject_Tool& rhs)
    : CUI_Object(rhs)
{
}

HRESULT CUIObject_Tool::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // GUI Inspector 창에 띄움
    CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;

    return S_OK;
}

void CUIObject_Tool::DestroyChild_FromParent()
{
    if (!m_pParent)
        return;

    auto pParentContainer = m_pParent->Get_Component<CObjectContainer>();
    if (!pParentContainer)
        return;

    pParentContainer->Destroy_Child(m_iChildIndex);
}

void CUIObject_Tool::ToJson(json& data)
{
    ToJson_Common(data);

    ToJson_Parent(data);
}

void CUIObject_Tool::FromJson(const json& data)
{
    FromJson_LinkParent(data);
}

void CUIObject_Tool::ToJson_Common(json& data)
{ 
    // 기본 정보
    data["levelTag"] = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(); // 현재 레벨을 가져와야하나 아니면 객체에 저장된 레벨을 가져와야하나
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
}

void CUIObject_Tool::ToJson_Parent(json& data)
{
    if (!m_pParent || -1 == m_iChildIndex)
        return;

    const string& strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    auto& Objects = CGameInstance::GetInstance()->Get_UIMgr()->Get_LevelUI(strCurrentLevel);

    string strParentInstanceKey;
    for (auto& pObj : Objects)
    {
        if (pObj == m_pParent)
        {
            strParentInstanceKey = pObj->Get_InstanceName();
            break;
        }
    }

    if (strParentInstanceKey.empty())
        return;

    data["parent"]["instanceKey"] = strParentInstanceKey;
    data["parent"]["childIndex"] = m_iChildIndex;
}

void CUIObject_Tool::FromJson_LinkParent(const json& data)
{
    if (!data.contains("parent") || !data["parent"].size())
        return;

    vector<CUI_Object*> objects = CGameInstance::GetInstance()->Get_UIMgr()->Get_LevelUI(data["levelTag"]);

    for (auto& pObj : objects)
    {
        if (data["parent"]["instanceKey"] == pObj->Get_InstanceName())
        {
            CUIObject_Tool* pUIObj = dynamic_cast<CUIObject_Tool*>(pObj);
            if (!pUIObj)
                break;

            m_pParent = pUIObj;
            m_iChildIndex = data["parent"]["childIndex"];

            pObj->Get_Component<CObjectContainer>()->Add_Child(this);

            break;
        }
    }
}

void CUIObject_Tool::Add_Child(CUIObject_Tool* pChild)
{
    if (!pChild || pChild == this)
        return;

    auto pContainer = Get_Component<CObjectContainer>();
   if (!pContainer)
       return;

   if (pChild->m_pParent)
       pChild->m_pParent->Remove_Child(pChild);

   _int iIndex = pContainer->Add_Child(pChild);
   if (-1 == iIndex)
       return;

   pChild->m_pParent = this;
   pChild->m_iChildIndex = iIndex;
}

void CUIObject_Tool::Remove_Child(CUIObject_Tool* pChild)
{
    auto pContainer = Get_Component<CObjectContainer>();
    if (!pContainer)
        return;

    _int index = pContainer->Find_IndexByName(pChild->Get_InstanceName());
    if (index == -1)
        return;

    pContainer->Destroy_Child(index);
    pChild->m_pParent = nullptr;
    pChild->m_iChildIndex = -1;
}

void CUIObject_Tool::FromJson_RefreshCount(_uint& iCount)
{
    int index = {};
    for (char c : Get_InstanceName())
    {
        if (isdigit(c))
            index = index * 10 + (c - '0');
    }

    iCount = max(iCount, index + 1);
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

void CUIObject_Tool::Change_Texture(_uint index, const string& levelKey, const string& TextureKey, string& strTextureKey)
{
    Get_Component<CSprite2D>()->Change_Texture(index, levelKey, TextureKey);
    strTextureKey = TextureKey;
}

void CUIObject_Tool::Free()
{
    __super::Free();
}