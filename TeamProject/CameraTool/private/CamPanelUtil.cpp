#include "pch.h"
#include "CamPanelUtil.h"

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
