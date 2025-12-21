#include "pch.h"
#include "CamPanelUtil.h"

bool CamPanelUtil::DrawEaseComboPopup(EaseType& ioValue, EaseType shownValue)
{
    bool changed = false;

    auto Pick = [&](EaseType v)
        {
            const bool selected = (shownValue == v);
            if (ImGui::Selectable(Helper::EnumLabel(v), selected))
            {
                ioValue = v;
                changed = true;
            }
            if (selected) ImGui::SetItemDefaultFocus();
        };

    Pick(EaseType::None);

    ImGui::SeparatorText("A. Stable");
    Pick(EaseType::InOutSine);
    Pick(EaseType::OutCubic);
    Pick(EaseType::InOutCubic);
    Pick(EaseType::OutSine);
    Pick(EaseType::InOutQuad);

    ImGui::SeparatorText("B. Ease In");
    Pick(EaseType::InSine);
    Pick(EaseType::InCubic);
    Pick(EaseType::InQuad);
    Pick(EaseType::InCirc);

    ImGui::SeparatorText("C. Settle / Stop");
    Pick(EaseType::InOutCirc);
    Pick(EaseType::OutCirc);
    Pick(EaseType::OutQuad);

    ImGui::SeparatorText("D. Strong");
    Pick(EaseType::InQuart);
    Pick(EaseType::InQuint);
    Pick(EaseType::InOutQuart);
    Pick(EaseType::OutQuart);
    Pick(EaseType::InOutQuint);
    Pick(EaseType::OutQuint);

    ImGui::SeparatorText("E. Extreme");
    Pick(EaseType::InOutExpo);
    Pick(EaseType::OutExpo);
    Pick(EaseType::InExpo);

    ImGui::SeparatorText("F. Overshoot");
    Pick(EaseType::OutBack);
    Pick(EaseType::InOutBack);
    Pick(EaseType::InBack);

    ImGui::SeparatorText("G. Special");
    Pick(EaseType::OutElastic);
    Pick(EaseType::InOutElastic);
    Pick(EaseType::InElastic);
    Pick(EaseType::OutBounce);
    Pick(EaseType::InOutBounce);
    Pick(EaseType::InBounce);

    return changed;
}

void CamPanelUtil::DrawEaseGraph(EaseType ease, ImVec2 size, const char* id)
{
    if (size.x <= 10.f) size.x = 360.f;
    if (size.y <= 10.f) size.y = 180.f;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 p1(p0.x + size.x, p0.y + size.y);

    ImGui::InvisibleButton(id, size);

    ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 colGrid = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colLinear = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colCurve = ImGui::GetColorU32(ImGuiCol_Text);
    ImU32 colWarn = ImGui::GetColorU32(ImGuiCol_ButtonActive);

    dl->AddRectFilled(p0, p1, colBg, 6.f);
    dl->AddRect(p0, p1, colBorder, 6.f);

    const int samples = 160;
    float minY = 1e9f;
    float maxY = -1e9f;

    for (int i = 0; i <= samples; ++i)
    {
        float u = (float)i / (float)samples;
        float y = Math::ApplyEase(ease, u);
        minY = min(minY, y);
        maxY = max(maxY, y);
    }

    float lo = minY;
    float hi = maxY;

    if (lo > 0.f) lo = 0.f;
    if (hi < 1.f) hi = 1.f;

    float pad = (hi - lo) * 0.08f;
    if (pad < 0.02f) pad = 0.02f;

    lo -= pad;
    hi += pad;

    lo = max(lo, -1.0f);
    hi = min(hi, 2.0f);

    auto ToScreen = [&](float u, float y) -> ImVec2
        {
            float y01 = (y - lo) / (hi - lo);
            y01 = clamp(y01, 0.f, 1.f);

            float x = p0.x + u * size.x;
            float ypix = p1.y - y01 * size.y;
            return ImVec2(x, ypix);
        };

    const int gridN = 4;
    for (int i = 1; i < gridN; ++i)
    {
        float t = (float)i / (float)gridN;
        float x = p0.x + t * size.x;
        float y = p0.y + t * size.y;

        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), colGrid, 1.f);
        dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), colGrid, 1.f);
    }

    dl->AddLine(ToScreen(0.f, 0.f), ToScreen(1.f, 1.f), colLinear, 1.5f);

    const bool overshoot = (minY < 0.f) || (maxY > 1.f);

    ImVec2 prev = ToScreen(0.f, Math::ApplyEase(ease, 0.f));
    for (int i = 1; i <= samples; ++i)
    {
        float u = (float)i / (float)samples;
        float y = Math::ApplyEase(ease, u);
        ImVec2 cur = ToScreen(u, y);
        dl->AddLine(prev, cur, overshoot ? colWarn : colCurve, 2.0f);
        prev = cur;
    }

    ImVec2 labelPos(p0.x + 10.f, p0.y + 8.f);
    dl->AddText(labelPos, ImGui::GetColorU32(ImGuiCol_Text), Helper::EnumLabel(ease));

    char rangeBuf[128];
    sprintf_s(rangeBuf, "y range: %.2f .. %.2f", minY, maxY);
    dl->AddText(ImVec2(labelPos.x, labelPos.y + 18.f), ImGui::GetColorU32(ImGuiCol_TextDisabled), rangeBuf);

    if (overshoot)
        dl->AddText(ImVec2(labelPos.x, labelPos.y + 36.f), colWarn, "Overshoot");
}

bool CamPanelUtil::TableCellHit(const char* id, float rowH, bool& outRowHovered, bool& outRowClicked)
{
    const ImGuiStyle& style = ImGui::GetStyle();

    ImVec2 p = ImGui::GetCursorScreenPos();
    float w = ImGui::GetContentRegionAvail().x;

    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y - style.CellPadding.y));
    ImGui::InvisibleButton(id, ImVec2(w, rowH));

    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked(0);

    if (hovered) outRowHovered = true;
    if (clicked) outRowClicked = true;

    ImGui::SetItemAllowOverlap();
    ImGui::SetCursorScreenPos(p);

    return clicked;
}

void CamPanelUtil::DrawLabelDisabled(const char* t)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("%s", t);
}

bool CamPanelUtil::DragFloat(const char* id, float& v, float speed, float minV, float maxV, const char* fmt, float valueW)
{
    ImGui::SetNextItemWidth(valueW);
    if (minV < maxV) return ImGui::DragFloat(id, &v, speed, minV, maxV, fmt);
    return ImGui::DragFloat(id, &v, speed, 0.f, 0.f, fmt);
}
