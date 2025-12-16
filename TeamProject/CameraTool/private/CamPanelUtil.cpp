#include "pch.h"
//#include "CamPanelUtil.h"
//#include "Helper_Func.h"
//
//
//    const char* CamPanelUtil::GetEaseLabel(CamEaseType v)
//    {
//        switch (v)
//        {
//        case (CamEaseType)0:  return "None";
//
//        case (CamEaseType)1:  return "InOutSine";
//        case (CamEaseType)2:  return "OutCubic";
//        case (CamEaseType)3:  return "InOutCubic";
//        case (CamEaseType)4:  return "OutSine";
//        case (CamEaseType)5:  return "InOutQuad";
//
//        case (CamEaseType)6:  return "InSine";
//        case (CamEaseType)7:  return "InCubic";
//        case (CamEaseType)8:  return "InQuad";
//        case (CamEaseType)9:  return "InCirc";
//
//        case (CamEaseType)10: return "InOutCirc";
//        case (CamEaseType)11: return "OutCirc";
//        case (CamEaseType)12: return "OutQuad";
//
//        case (CamEaseType)13: return "InQuart";
//        case (CamEaseType)14: return "InQuint";
//        case (CamEaseType)15: return "InOutQuart";
//        case (CamEaseType)16: return "OutQuart";
//        case (CamEaseType)17: return "InOutQuint";
//        case (CamEaseType)18: return "OutQuint";
//
//        case (CamEaseType)19: return "InOutExpo";
//        case (CamEaseType)20: return "OutExpo";
//        case (CamEaseType)21: return "InExpo";
//
//        case (CamEaseType)22: return "OutBack";
//        case (CamEaseType)23: return "InOutBack";
//        case (CamEaseType)24: return "InBack";
//
//        case (CamEaseType)25: return "OutElastic";
//        case (CamEaseType)26: return "InOutElastic";
//        case (CamEaseType)27: return "InElastic";
//
//        case (CamEaseType)28: return "OutBounce";
//        case (CamEaseType)29: return "InOutBounce";
//        case (CamEaseType)30: return "InBounce";
//
//        default:              return "Unknown";
//        }
//    }
//
//    bool DrawEaseComboPopup(CamEaseType& ioValue, CamEaseType shownValue)
//    {
//        bool changed = false;
//
//        auto Pick = [&](CamEaseType v)
//            {
//                const bool selected = (shownValue == v);
//                if (ImGui::Selectable(GetEaseLabel(v), selected))
//                {
//                    ioValue = v;
//                    changed = true;
//                }
//                if (selected) ImGui::SetItemDefaultFocus();
//            };
//
//        Pick((CamEaseType)0);
//
//        ImGui::SeparatorText("A. Stable");
//        Pick((CamEaseType)1);
//        Pick((CamEaseType)2);
//        Pick((CamEaseType)3);
//        Pick((CamEaseType)4);
//        Pick((CamEaseType)5);
//
//        ImGui::SeparatorText("B. Ease In");
//        Pick((CamEaseType)6);
//        Pick((CamEaseType)7);
//        Pick((CamEaseType)8);
//        Pick((CamEaseType)9);
//
//        ImGui::SeparatorText("C. Settle / Stop");
//        Pick((CamEaseType)10);
//        Pick((CamEaseType)11);
//        Pick((CamEaseType)12);
//
//        ImGui::SeparatorText("D. Strong");
//        Pick((CamEaseType)13);
//        Pick((CamEaseType)14);
//        Pick((CamEaseType)15);
//        Pick((CamEaseType)16);
//        Pick((CamEaseType)17);
//        Pick((CamEaseType)18);
//
//        ImGui::SeparatorText("E. Extreme");
//        Pick((CamEaseType)19);
//        Pick((CamEaseType)20);
//        Pick((CamEaseType)21);
//
//        ImGui::SeparatorText("F. Overshoot");
//        Pick((CamEaseType)22);
//        Pick((CamEaseType)23);
//        Pick((CamEaseType)24);
//
//        ImGui::SeparatorText("G. Special");
//        Pick((CamEaseType)25);
//        Pick((CamEaseType)26);
//        Pick((CamEaseType)27);
//        Pick((CamEaseType)28);
//        Pick((CamEaseType)29);
//        Pick((CamEaseType)30);
//
//        return changed;
//    }
//
//    float EvalEase(CamEaseType v, float u)
//    {
//        u = std::clamp(u, 0.f, 1.f);
//
//        switch (v)
//        {
//        case (CamEaseType)0:  return u;
//        case (CamEaseType)1:  return Math::EaseInOutSine(u);
//        case (CamEaseType)2:  return Math::EaseOutCubic(u);
//        case (CamEaseType)3:  return Math::EaseInOutCubic(u);
//        case (CamEaseType)4:  return Math::EaseOutSine(u);
//        case (CamEaseType)5:  return Math::EaseInOutQuad(u);
//
//        case (CamEaseType)6:  return Math::EaseInSine(u);
//        case (CamEaseType)7:  return Math::EaseInCubic(u);
//        case (CamEaseType)8:  return Math::EaseInQuad(u);
//        case (CamEaseType)9:  return Math::EaseInCirc(u);
//
//        case (CamEaseType)10: return Math::EaseInOutCirc(u);
//        case (CamEaseType)11: return Math::EaseOutCirc(u);
//        case (CamEaseType)12: return Math::EaseOutQuad(u);
//
//        case (CamEaseType)13: return Math::EaseInQuart(u);
//        case (CamEaseType)14: return Math::EaseInQuint(u);
//        case (CamEaseType)15: return Math::EaseInOutQuart(u);
//        case (CamEaseType)16: return Math::EaseOutQuart(u);
//        case (CamEaseType)17: return Math::EaseInOutQuint(u);
//        case (CamEaseType)18: return Math::EaseOutQuint(u);
//
//        case (CamEaseType)19: return Math::EaseInOutExpo(u);
//        case (CamEaseType)20: return Math::EaseOutExpo(u);
//        case (CamEaseType)21: return Math::EaseInExpo(u);
//
//        case (CamEaseType)22: return Math::EaseOutBack(u);
//        case (CamEaseType)23: return Math::EaseInOutBack(u);
//        case (CamEaseType)24: return Math::EaseInBack(u);
//
//        case (CamEaseType)25: return Math::EaseOutElastic(u);
//        case (CamEaseType)26: return Math::EaseInOutElastic(u);
//        case (CamEaseType)27: return Math::EaseInElastic(u);
//
//        case (CamEaseType)28: return Math::EaseOutBounce(u);
//        case (CamEaseType)29: return Math::EaseInOutBounce(u);
//        case (CamEaseType)30: return Math::EaseInBounce(u);
//
//        default:              return u;
//        }
//    }
//
//    void DrawEaseGraph(CamEaseType ease, ImVec2 size)
//    {
//        if (size.x <= 10.f) size.x = 360.f;
//        if (size.y <= 10.f) size.y = 180.f;
//
//        ImDrawList* dl = ImGui::GetWindowDrawList();
//        ImVec2 p0 = ImGui::GetCursorScreenPos();
//        ImVec2 p1(p0.x + size.x, p0.y + size.y);
//
//        ImGui::InvisibleButton("##ease_graph", size);
//
//        ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
//        ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
//        ImU32 colGrid = ImGui::GetColorU32(ImGuiCol_TextDisabled);
//        ImU32 colLinear = ImGui::GetColorU32(ImGuiCol_TextDisabled);
//        ImU32 colCurve = ImGui::GetColorU32(ImGuiCol_Text);
//        ImU32 colWarn = ImGui::GetColorU32(ImGuiCol_ButtonActive);
//
//        dl->AddRectFilled(p0, p1, colBg, 6.f);
//        dl->AddRect(p0, p1, colBorder, 6.f);
//
//        const int samples = 160;
//        float minY = 1e9f;
//        float maxY = -1e9f;
//
//        for (int i = 0; i <= samples; ++i)
//        {
//            float u = (float)i / (float)samples;
//            float y = EvalEase(ease, u);
//            minY = std::min(minY, y);
//            maxY = std::max(maxY, y);
//        }
//
//        float lo = minY;
//        float hi = maxY;
//
//        if (lo > 0.f) lo = 0.f;
//        if (hi < 1.f) hi = 1.f;
//
//        float pad = (hi - lo) * 0.08f;
//        if (pad < 0.02f) pad = 0.02f;
//
//        lo -= pad;
//        hi += pad;
//
//        lo = std::max(lo, -1.0f);
//        hi = std::min(hi, 2.0f);
//
//        auto ToScreen = [&](float u, float y) -> ImVec2
//            {
//                float x01 = u;
//                float y01 = (y - lo) / (hi - lo);
//                y01 = std::clamp(y01, 0.f, 1.f);
//
//                float x = p0.x + x01 * size.x;
//                float ypix = p1.y - y01 * size.y;
//                return ImVec2(x, ypix);
//            };
//
//        const int gridN = 4;
//        for (int i = 1; i < gridN; ++i)
//        {
//            float t = (float)i / (float)gridN;
//            float x = p0.x + t * size.x;
//            float y = p0.y + t * size.y;
//            dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), colGrid, 1.f);
//            dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), colGrid, 1.f);
//        }
//
//        dl->AddLine(ToScreen(0.f, 0.f), ToScreen(1.f, 1.f), colLinear, 1.5f);
//
//        bool overshoot = (minY < 0.f) || (maxY > 1.f);
//
//        ImVec2 prev = ToScreen(0.f, EvalEase(ease, 0.f));
//        for (int i = 1; i <= samples; ++i)
//        {
//            float u = (float)i / (float)samples;
//            float y = EvalEase(ease, u);
//            ImVec2 cur = ToScreen(u, y);
//            dl->AddLine(prev, cur, overshoot ? colWarn : colCurve, 2.0f);
//            prev = cur;
//        }
//
//        ImVec2 labelPos(p0.x + 10.f, p0.y + 8.f);
//        dl->AddText(labelPos, ImGui::GetColorU32(ImGuiCol_Text), GetEaseLabel(ease));
//
//        char rangeBuf[128];
//        sprintf_s(rangeBuf, "y range: %.2f .. %.2f", minY, maxY);
//        dl->AddText(ImVec2(labelPos.x, labelPos.y + 18.f), ImGui::GetColorU32(ImGuiCol_TextDisabled), rangeBuf);
//
//        if (overshoot)
//            dl->AddText(ImVec2(labelPos.x, labelPos.y + 36.f), colWarn, "Overshoot");
//    }
//
//    bool TableCellHit(const char* id, float rowH, bool& outRowHovered, bool& outRowClicked)
//    {
//        const ImGuiStyle& style = ImGui::GetStyle();
//
//        ImVec2 p = ImGui::GetCursorScreenPos();
//        float w = ImGui::GetContentRegionAvail().x;
//
//        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y - style.CellPadding.y));
//        ImGui::InvisibleButton(id, ImVec2(w, rowH));
//
//        bool hovered = ImGui::IsItemHovered();
//        bool clicked = ImGui::IsItemClicked(0);
//
//        if (hovered) outRowHovered = true;
//        if (clicked) outRowClicked = true;
//
//        ImGui::SetItemAllowOverlap();
//        ImGui::SetCursorScreenPos(p);
//
//        return clicked;
//    }
//
//    ConfirmResult DrawConfirmPopupModal(
//        const char* popupId,
//        const char* title,
//        std::initializer_list<const char*> lines,
//        const char* okLabel,
//        const char* cancelLabel,
//        float buttonW)
//    {
//        if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
//            return ConfirmResult::None;
//
//        if (title && title[0])
//        {
//            ImGui::TextUnformatted(title);
//            ImGui::Separator();
//        }
//
//        for (auto* s : lines)
//            ImGui::TextUnformatted(s);
//
//        ImGui::Separator();
//
//        ConfirmResult r = ConfirmResult::None;
//
//        if (ImGui::Button(okLabel, ImVec2(buttonW, 0.f)))
//        {
//            r = ConfirmResult::Ok;
//            ImGui::CloseCurrentPopup();
//        }
//
//        ImGui::SameLine();
//
//        if (ImGui::Button(cancelLabel, ImVec2(buttonW, 0.f)))
//        {
//            r = ConfirmResult::Cancel;
//            ImGui::CloseCurrentPopup();
//        }
//
//        ImGui::EndPopup();
//        return r;
//    }
//
//    bool DrawOkPopupModal( const char* popupId, const char* title, initializer_list<const char*> lines, const char* okLabel, float buttonW)
//    {
//        if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
//            return false;
//
//        if (title && title[0])
//        {
//            ImGui::TextUnformatted(title);
//            ImGui::Separator();
//        }
//
//        for (auto* s : lines)
//            ImGui::TextUnformatted(s);
//
//        ImGui::Separator();
//
//        bool closed = false;
//        if (ImGui::Button(okLabel, ImVec2(buttonW, 0.f)))
//        {
//            closed = true;
//            ImGui::CloseCurrentPopup();
//        }
//
//        ImGui::EndPopup();
//        return closed;
//    }
//
//    bool DrawOkPopupModalText( const char* popupId, const char* title, const string& bodyText, const char* okLabel, float buttonW)
//    {
//        if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
//            return false;
//
//        if (title && title[0])
//        {
//            ImGui::TextUnformatted(title);
//            ImGui::Separator();
//        }
//
//        if (!bodyText.empty())
//            ImGui::TextUnformatted(bodyText.c_str());
//
//        ImGui::Separator();
//
//        bool closed = false;
//        if (ImGui::Button(okLabel, ImVec2(buttonW, 0.f)))
//        {
//            closed = true;
//            ImGui::CloseCurrentPopup();
//        }
//
//        ImGui::EndPopup();
//        return closed;
//    }