#include "pch.h"
#include "TextUI.h"

#include "Sprite2D.h"
#include "TextSlot.h"
#include "UITool_Level.h"
#include "Helper_Func.h"

HRESULT CTextUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CTextSlot>();

    return S_OK;
}

HRESULT CTextUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto pSprite = Get_Component<CSprite2D>();
    auto pTextSlot = Get_Component<CTextSlot>();

    const auto& szFontKeys = CUITool_Level::m_strFontKeys;
    if (szFontKeys.size())
    {
        m_strFontTag = szFontKeys[m_iFontKeyIndex];
        pTextSlot->Set_Font(m_strFontTag);
    }

    strcpy_s(m_szText, sizeof(m_szText), u8"text");
    pTextSlot->Set_Text(Helper::ConvertToWideString(m_szText));

    const string strObjectID = to_string(m_ObjectID).c_str();
    pSprite->Set_TextKey(strObjectID);
    pTextSlot->Set_TextKey(strObjectID);

    pTextSlot->Set_Color(m_vColor);

    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

//#ifdef _DEBUG
    pSprite->Add_Texture(G_GlobalLevelKey, "canvas.png");
//#else
//    pSprite->Add_Texture(G_GlobalLevelKey, "transparent.png");
//#endif

    m_iCount++;

    return S_OK;
}

void CTextUI::Update(_float dt)
{
    __super::Update(dt);

    auto pTextSlot = Get_Component<CTextSlot>();

    if (pTextSlot->Is_AutoPos())
    {
        _float2 vPivot = m_vLeftTop;
        _uint anchorFlags = static_cast<_uint>(m_eAutoPosAnchor);

        if (anchorFlags & static_cast<_uint>(ANCHOR::Left))
        {
        }
        else if (anchorFlags & static_cast<_uint>(ANCHOR::Right))
            vPivot.x += m_vSize.x;
        else
            vPivot.x += m_vSize.x * 0.5f;

        if (anchorFlags & static_cast<_uint>(ANCHOR::Top))
        {
        }
        else if (anchorFlags & static_cast<_uint>(ANCHOR::Bottom))
            vPivot.y += m_vSize.y;
        else
            vPivot.y += m_vSize.y * 0.5f;

        pTextSlot->Update_Pivot(vPivot);

    } 
    else
        pTextSlot->Set_Position(m_vLeftTop);

    pTextSlot->Push_Text();
    pTextSlot->Set_Color(_float4(m_vColor.x, m_vColor.y, m_vColor.z, m_fCombinedAlpha));
}

void CTextUI::Render_GUI()
{
    Render_GUI_Property();
    Render_GUI_Layout();
    Render_GUI_Transform();
    Render_GUI_Animation();
    Render_GUI_Text();
}

void CTextUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;

    auto& textJson               = data["text"];
    textJson["content"]          = m_szText;
    textJson["fontTag"]          = m_strFontTag;
    textJson["fontScale"]        = m_fFontScale;
    textJson["outlined"]         = m_isOutlined;
    textJson["outlineThickness"] = m_fOutlineThickness;
    textJson["outlineColor"]     = { m_vOutlineColor.x, m_vOutlineColor.y, m_vOutlineColor.z, m_vOutlineColor.w };
    textJson["shear"]            = { m_vShear.x, m_vShear.y };
    textJson["autoPos"]         = m_isAutoPos;
    textJson["anchor"]           = ENUM(m_eAutoPosAnchor);
}

void CTextUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    if (data.contains("text"))
    {
        auto pTextSlot = Get_Component<CTextSlot>();
        const auto& textJson = data["text"];

        strcpy_s(m_szText, textJson.value("content", "").c_str());
        m_strFontTag        = textJson.value("fontTag", "DefaultFont");
        m_fFontScale        = textJson.value("fontScale", 1.0f);
        m_isOutlined        = textJson.value("outlined", false);
        m_fOutlineThickness = textJson.value("outlineThickness", 1.0f); 
        auto outlineColor   = textJson.value("outlineColor", json::array({ 0.0f, 0.0f, 0.0f, 1.0f }));
        m_vOutlineColor     = { outlineColor[0], outlineColor[1], outlineColor[2], outlineColor[3] };
        auto vShear         = textJson.value("shear", json::array({ 0.0f, 0.0f }));
        m_vShear            = {vShear[0], vShear[1]};

        pTextSlot->Set_Font(m_strFontTag);
        pTextSlot->Set_Text(Helper::ConvertToWideString(m_szText));
        pTextSlot->Set_Size(m_fFontScale);
        pTextSlot->Set_Color(m_vColor);
        if (m_isOutlined)
            pTextSlot->Set_OutLine(m_fOutlineThickness, m_vOutlineColor);
        pTextSlot->Set_Shear(m_vShear);

        m_isAutoPos = textJson.value("autoPos", true);
        if (m_isAutoPos)
        {
            m_eAutoPosAnchor = static_cast<ANCHOR>(textJson.value("anchor", 0));
            pTextSlot->Enable_AutoPos(m_eAutoPosAnchor);
        } 
    } 
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
        Set_Pivot(tmpPivot);

    ImGui::TextDisabled("LeftTop : %.1f, %.1f", m_vLeftTop.x, m_vLeftTop.y);
    ImGui::TextDisabled("WinSize : %.1f x %.1f", m_WinSize.x, m_WinSize.y);
}

void CTextUI::Render_GUI_Text()
{
    Render_GUI_TextAlign();

    // 내용
    ImGui::SeparatorText(u8"텍스트 내용");
    if (ImGui::InputTextMultiline(u8"##내용", static_cast<_char*>(m_szText), sizeof(m_szText), ImVec2(ImGui::GetContentRegionAvail().x, 50.f)))
        Get_Component<CTextSlot>()->Set_Text(Helper::ConvertToWideString(m_szText));

    // 폰트 
    ImGui::SeparatorText(u8"폰트");
    const auto& szFontKeys = CUITool_Level::m_szFontKeys;
    if (ImGui::Combo(u8"##폰트", &m_iFontKeyIndex, szFontKeys.data(), szFontKeys.size()))
        m_strFontTag = szFontKeys[m_iFontKeyIndex];
        Get_Component<CTextSlot>()->Set_Font(m_strFontTag);

    // 크기
    ImGui::SeparatorText(u8"크기");
    if (ImGui::Button(u8"+"))
    {
        m_fFontScale = min(m_fFontScale + 0.05f, 2.f);
        Get_Component<CTextSlot>()->Set_Size(m_fFontScale);
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"-"))
    {
        m_fFontScale = max(m_fFontScale - 0.05f, 0.1f);
        Get_Component<CTextSlot>()->Set_Size(m_fFontScale);
    }

    // 컬러
    ImGui::SeparatorText(u8"텍스트 컬러");
    ImGui::ColorEdit4(u8"##텍스트 컬러", reinterpret_cast<_float*>(&m_vColor));

    // 기울기
    ImGui::SeparatorText(u8"텍스트 기울기");
    if (ImGui::DragFloat2(u8"##텍스트 기울기", reinterpret_cast<_float*>(&m_vShear), 0.1f, -1.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        Get_Component<CTextSlot>()->Set_Shear(m_vShear);

    // 외곽선
    ImGui::SeparatorText(u8"외곽선");
    _bool isChanged = {};
    isChanged |= ImGui::Checkbox(u8"##외곽선", &m_isOutlined);
    isChanged |= ImGui::DragFloat(u8"굵기", &m_fOutlineThickness, 0.1f, 0.f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    isChanged |= ImGui::ColorEdit4(u8"컬러", reinterpret_cast<_float*>(&m_vOutlineColor));

    if (isChanged)
    {
        if (m_isOutlined)
            Get_Component<CTextSlot>()->Set_OutLine(m_fOutlineThickness, m_vOutlineColor);
        else
            Get_Component<CTextSlot>()->ReSet_OutLine();
    }
}

void CTextUI::Render_GUI_TextAlign()
{
    ImGui::SeparatorText(u8"텍스트 정렬");
    ImGui::Checkbox(u8"자동 정렬", &m_isAutoPos);

    if (!m_isAutoPos)
    {
        Get_Component<CTextSlot>()->Disable_AutoPos();
        return;
    }
    
    Get_Component<CTextSlot>()->Enable_AutoPos(m_eAutoPosAnchor);

    ImGui::Text("Anchor");
    struct AnchorPreset { const char* label; ANCHOR value; };
    static const AnchorPreset presets[9] = {
        {"##AUTOTL", (ANCHOR)((_uint)ANCHOR::Top | (_uint)ANCHOR::Left)},
        {"##AUTOTC", (ANCHOR)((_uint)ANCHOR::Top)},
        {"##AUTOTR", (ANCHOR)((_uint)ANCHOR::Top | (_uint)ANCHOR::Right)},

        {"##AUTOCL", (ANCHOR)((_uint)ANCHOR::Left)},
        {"##AUTOCC", (ANCHOR)((_uint)ANCHOR::Center)},
        {"##AUTOCR", (ANCHOR)((_uint)ANCHOR::Right)},

        {"##AUTOBL", (ANCHOR)((_uint)ANCHOR::Bottom | (_uint)ANCHOR::Left)},
        {"##AUTOBC", (ANCHOR)((_uint)ANCHOR::Bottom)},
        {"##AUTOBR", (ANCHOR)((_uint)ANCHOR::Bottom | (_uint)ANCHOR::Right)},
    };

    auto IsSelected = [&](ANCHOR a) { return (_uint)a == (_uint)m_eAutoPosAnchor; };

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
            m_eAutoPosAnchor = presets[i].value;

        if (selected) ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::TextDisabled("Selected: %u", (_uint)m_eAutoPosAnchor);
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