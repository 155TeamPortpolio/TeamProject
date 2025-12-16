//#pragma once
//
//NS_BEGIN(CameraTool)
//
//namespace CamPanelUtil
//{
//	enum class ConfirmResult
//    {
//        None, Ok, Cancel
//    };
//
//    const char*   GetEaseLabel(CamEaseType v);
//    bool          DrawEaseComboPopup(CamEaseType& ioValue, CamEaseType shownValue);
//    float         EvalEase(CamEaseType v, float u);
//    void          DrawEaseGraph(CamEaseType ease, ImVec2 size);
//
//    bool          TableCellHit(const char* id, float rowH, bool& outRowHovered, bool& outRowClicked);
//
//    ConfirmResult DrawConfirmPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines, const char* okLabel, const char* cancelLabel, float buttonW = 120.f);
//
//    bool DrawOkPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines, 
//        const char* okLabel = "OK", float buttonW = 120.f);
//
//    bool DrawOkPopupModalText( const char* popupId, const char* title, const string& bodyText, const char* okLabel = "OK", float buttonW = 120.f);
//}
//
//NS_END