#pragma once

NS_BEGIN(CameraTool)

enum class ConfirmResult
{
    None, Ok, Cancel
};

namespace CamPanelUtil
{
    bool DrawEaseComboPopup(EaseType& ioValue, EaseType shownValue);

    void DrawEaseGraph(EaseType ease, ImVec2 size, const char* id = "##ease_graph");

    bool TableCellHit(const char* id, float rowH, bool& outRowHovered, bool& outRowClicked);

    ConfirmResult DrawConfirmPopupModal(const char* popupId, const char* title, 
        initializer_list<const char*> lines, const char* okLabel, const char* cancelLabel, float buttonW = 120.f);

    bool DrawOkPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines,
        const char* okLabel = "OK", float buttonW = 120.f);

    bool DrawOkPopupModalText(const char* popupId, const char* title, const string& bodyText, const char* okLabel = "OK", float buttonW = 120.f);

    const char* GetPosInterpLabel(CamPosInterp v);
    const char* GetRotInterpLabel(CamRotInterp v);
    const char* GetFovInterpLabel(CamFovInterp v);

    const char* GetMoveConstraintLabel(CamMoveConstraint v);
    const char* GetOrbitArcAngleLabel(CamOrbitArcAngleMode v);
    const char* GetOrbitArcRadiusLabel(CamOrbitArcRadiusMode v);
}

NS_END