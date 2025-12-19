#pragma once

NS_BEGIN(CameraTool)

namespace CamPanelUtil
{
    bool DrawEaseComboPopup(EaseType& ioValue, EaseType shownValue);
    void DrawEaseGraph(EaseType ease, ImVec2 size, const char* id = "##ease_graph");
    bool TableCellHit(const char* id, float rowH, bool& outRowHovered, bool& outRowClicked);
}

NS_END