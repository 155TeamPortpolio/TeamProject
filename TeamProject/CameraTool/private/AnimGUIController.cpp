#include "pch.h"
#include "AnimGUIController.h"
#include "Animator3D.h"

namespace
{
    string MakeAnimClipShortLabel(const string& full)
    {
        const size_t p = full.rfind("_Ani_");
        if (p != string::npos && p + 5 < full.size())
            return full.substr(p + 5);

        const size_t p2 = full.rfind("Ani_");
        if (p2 != string::npos && p2 + 4 < full.size())
            return full.substr(p2 + 4);

        return full;
    }
}

CAnimator3D* CAnimGUIController::ResolveAnimator(OBJECT_HANDLE spaceRefHandle) const
{
    auto refObj = ObjectManager()->Request_Object(spaceRefHandle);
    if (!refObj) return nullptr;

    return refObj->Get_Component<CAnimator3D>();
}

void CAnimGUIController::DrawScrubBar(CAnimator3D* anim, float width)
{
    const float durT = anim->Get_DurationSec();
    if (durT <= 0.f) return;

    const float curT = anim->Get_TimeSec();
    const float t01 = clamp(curT / durT, 0.f, 1.f);

    const float h = ImGui::GetFrameHeight();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(width, h);

    ImGui::InvisibleButton("##anim_scrub", size);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 colFill = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    ImU32 colCursor = ImGui::GetColorU32(ImGuiCol_Text);
    ImU32 colText = ImGui::GetColorU32(ImGuiCol_TextDisabled);

    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), colBg, 4.f);
    dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), colBorder, 4.f);
    dl->AddRectFilled(pos, ImVec2(pos.x + size.x * t01, pos.y + size.y), colFill, 4.f);

    const float cx = pos.x + size.x * t01;
    dl->AddLine(ImVec2(cx, pos.y - 2.f), ImVec2(cx, pos.y + size.y + 2.f), colCursor, 2.f);

    char buf[64];
    sprintf_s(buf, "%.2f / %.2f", curT, durT);
    dl->AddText(ImVec2(pos.x + 6.f, pos.y + (size.y - ImGui::CalcTextSize(buf).y) * 0.5f), colText, buf);

    if (ImGui::IsItemActivated())
    {
        m_scrubAnim = anim;
        m_resumeAfterScrub = !anim->Get_isPause(0);
        if (m_resumeAfterScrub) anim->Set_Pause(true, 0);
    }

    if (ImGui::IsItemActive())
    {
        const float mx = ImGui::GetIO().MousePos.x;
        const float local01 = clamp((mx - pos.x) / size.x, 0.f, 1.f);
        anim->Set_TimeSec(local01 * durT);
    }

    if (ImGui::IsItemDeactivated() && m_scrubAnim == anim)
    {
        if (m_resumeAfterScrub) anim->Set_Pause(false, 0);
        m_scrubAnim = nullptr;
        m_resumeAfterScrub = false;
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("t = %.2f / %.2f", curT, durT);
}

void CAnimGUIController::DrawInline(OBJECT_HANDLE spaceRefHandle)
{
    auto anim = ResolveAnimator(spaceRefHandle);

    const bool hasAnim = anim != nullptr;
    const bool playing = hasAnim && !anim->Get_isPause(0);

    ImGui::SameLine(0.f, 8.f);

    if (playing)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.15f, 0.15f, 1.f));

    if (!hasAnim) ImGui::BeginDisabled();
    if (ImGui::SmallButton("Play"))
        anim->Set_Pause(false, 0);
    if (!hasAnim) ImGui::EndDisabled();

    if (playing)
        ImGui::PopStyleColor();

    ImGui::SameLine(0.f, 4.f);

    if (!hasAnim || !playing) ImGui::BeginDisabled();
    if (ImGui::SmallButton("Stop"))
        anim->Set_Pause(true, 0);
    if (!hasAnim || !playing) ImGui::EndDisabled();

    ImGui::SameLine(0.f, 4.f);

    if (!hasAnim) ImGui::BeginDisabled();
    if (ImGui::SmallButton("Reset"))
    {
        anim->Set_TimeSec(0.f);
        anim->Set_Pause(true, 0);
    }
    if (!hasAnim) ImGui::EndDisabled();

    ImGui::SameLine(0.f, 8.f);

    if (!hasAnim) ImGui::BeginDisabled();

    const _int clipCount = hasAnim ? anim->Get_AnimClipCount() : 0;
    const _int curIndex = hasAnim ? anim->Get_CurAnimIndex(0) : -1;

    string previewFull = "None";
    if (hasAnim && curIndex >= 0)
        previewFull = anim->Get_AnimClipName((_uint)curIndex);

    const string previewShort = MakeAnimClipShortLabel(previewFull);

    struct ClipItem
    {
        _int   index;
        string full;
        string shortLabel;
    };

    vector<ClipItem> items;
    items.reserve((size_t)clipCount);

    for (_int i = 0; i < clipCount; ++i)
    {
        ClipItem it{};
        it.index = i;
        it.full = anim->Get_AnimClipName((_uint)i);
        it.shortLabel = MakeAnimClipShortLabel(it.full);
        items.push_back(std::move(it));
    }

    sort(items.begin(), items.end(), [](const ClipItem& a, const ClipItem& b)
        {
            return _stricmp(a.shortLabel.c_str(), b.shortLabel.c_str()) < 0;
        });

    ImGui::SetNextItemWidth(220.f);
    if (ImGui::BeginCombo("##ref_anim_combo", previewShort.c_str()))
    {
        for (const auto& it : items)
        {
            const bool selected = (it.index == curIndex);

            string itemLabel = it.shortLabel;
            itemLabel += "##clip_";
            itemLabel += to_string(it.index);

            if (ImGui::Selectable(itemLabel.c_str(), selected))
            {
                anim->Change_Animation(0, it.index).Loop(true).Apply();
                anim->Set_Pause(false, 0);
                anim->Update_Animation(0.f);
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", it.full.c_str());

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (!hasAnim) ImGui::EndDisabled();
    if (!hasAnim) return;

    ImGui::SameLine(0.f, 8.f);
    ImGui::PushID(anim);
    DrawScrubBar(anim, 180.f);
    ImGui::PopID();
}

bool CAnimGUIController::HasAnimator(OBJECT_HANDLE spaceRefHandle) const
{
    return ResolveAnimator(spaceRefHandle) != nullptr;
}

bool CAnimGUIController::IsPlaying(OBJECT_HANDLE spaceRefHandle) const
{
    auto anim = ResolveAnimator(spaceRefHandle);
    return anim && !anim->Get_isPause(0);
}

void CAnimGUIController::SetPlaying(OBJECT_HANDLE spaceRefHandle, bool play)
{
    auto anim = ResolveAnimator(spaceRefHandle);
    anim->Set_Pause(!play, 0);
}

void CAnimGUIController::SetTimeSec(OBJECT_HANDLE spaceRefHandle, float timeSec)
{
    auto anim = ResolveAnimator(spaceRefHandle);
    anim->Set_TimeSec(timeSec);
    anim->Update_Animation(0.f);
}

float CAnimGUIController::GetClipEndSec(OBJECT_HANDLE spaceRefHandle) const
{
    auto anim = ResolveAnimator(spaceRefHandle);
    if (!anim) return 0.f;

    const float durT = anim->Get_DurationSec();
    if (durT <= 0.f) return 0.f;

    return durT;
}