#include "pch.h"
#include "CamPanel.h"

namespace
{
    void DrawFieldHeaderButton(const char* label, float width = 70.f)
    {
        ImGui::Button(label, ImVec2(width, 0.f));
        ImGui::SameLine();
    }
    bool DrawVec3Editor(const char* headerLabel, _vector3& v, float speed = 0.05f, float headerWidth = 70.f)
    {
        DrawFieldHeaderButton(headerLabel, headerWidth);

        bool changed = false;
        ImGui::PushID(headerLabel);

        ImGui::SetNextItemWidth(80.f);
        changed |= ImGui::DragFloat("X", &v.x, speed); ImGui::SameLine();
        ImGui::SetNextItemWidth(80.f);
        changed |= ImGui::DragFloat("Y", &v.y, speed); ImGui::SameLine();
        ImGui::SetNextItemWidth(80.f);
        changed |= ImGui::DragFloat("Z", &v.z, speed);

        ImGui::PopID();
        return changed;
    }
    bool DrawFloatEditor(const char* headerLabel, float& value, float speed = 0.1f, float minV = 0.f, float maxV = 0.f, float headerWidth = 70.f)
    {
        DrawFieldHeaderButton(headerLabel, headerWidth);

        ImGui::PushID(headerLabel);           
        ImGui::SetNextItemWidth(160.f);

        bool changed = false;
        if (minV < maxV)
            changed = ImGui::DragFloat("##val", &value, speed, minV, maxV);
        else
            changed = ImGui::DragFloat("##val", &value, speed);

        ImGui::PopID();                      
        return changed;
    }
    struct ScopedCamToolStyle final
    {
        ScopedCamToolStyle()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha,          1.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,   ImVec2(10.f,  7.f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,    ImVec2(8.f,   8.f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,  4.f);
            ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize,  16.f);

            ImGui::PushStyleColor(ImGuiCol_WindowBg,          ImVec4(0.06f, 0.06f, 0.06f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ChildBg,           ImVec4(0.09f, 0.09f, 0.09f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_Border,            ImVec4(0.30f, 0.30f, 0.30f, 0.85f));
            ImGui::PushStyleColor(ImGuiCol_Separator,         ImVec4(0.35f, 0.35f, 0.35f, 0.70f));

            ImGui::PushStyleColor(ImGuiCol_FrameBg,           ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,    ImVec4(0.22f, 0.22f, 0.22f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive,     ImVec4(0.28f, 0.28f, 0.28f, 1.00f));

            ImGui::PushStyleColor(ImGuiCol_Button,            ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,     ImVec4(0.20f, 0.20f, 0.20f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,      ImVec4(0.28f, 0.28f, 0.28f, 1.00f));

            ImGui::PushStyleColor(ImGuiCol_Header,            ImVec4(0.14f, 0.14f, 0.14f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered,     ImVec4(0.22f, 0.22f, 0.22f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive,      ImVec4(0.28f, 0.28f, 0.28f, 1.00f));

            ImGui::PushStyleColor(ImGuiCol_TableHeaderBg,     ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ImVec4(0.30f, 0.30f, 0.30f, 0.85f));
            ImGui::PushStyleColor(ImGuiCol_TableBorderLight,  ImVec4(0.25f, 0.25f, 0.25f, 0.65f));
        }

        ~ScopedCamToolStyle()
        {
            ImGui::PopStyleColor(16);
            ImGui::PopStyleVar(6);
        }

        ScopedCamToolStyle(const ScopedCamToolStyle&) = delete;
        ScopedCamToolStyle& operator=(const ScopedCamToolStyle&) = delete;
    };
}

void CamPanel::Init()
{
    debugSequence.name         = "DebugSequence";
    debugSequence.camType      = CamType::Cinematic;
    debugSequence.rigType      = CamRigType::Free;
    debugSequence.projType     = CamProjType::Perspective;
    debugSequence.playbackMode = CamPlaybackMode::Once;

    target.sequence = &debugSequence;
}

void CamPanel::Update_Panel(_float dt)
{
    if (!target.sequence)
    {
        state.selectedKeyIdx = -1;
        state.endTime = 0.f;
        state.curTime = 0.f;
        state.playing = false;
        return;
    }

    const auto& keys = GetKeys();

    float maxT = 0.f;
    for (size_t i = 0; i < keys.size(); ++i)
        maxT = max(maxT, keys[i].time);

    state.endTime = maxT;

    if (state.curTime < 0.f)           state.curTime = 0.f;
    if (state.curTime > state.endTime) state.curTime = state.endTime;

    if (state.playing)
    {
        if (state.endTime <= 1e-6f)
        {
            state.curTime = 0.f;
            state.playing = false;
        }
        else
        {
            state.curTime += dt;

            if (state.curTime >= state.endTime)
            {
                if (state.loop)
                    state.curTime = fmodf(state.curTime, state.endTime);
                else
                {
                    state.curTime = state.endTime;
                    state.playing = false;
                }
            }
        }
    }

    ApplyPlaybackAtTime(state.curTime);
}

void CamPanel::Render_GUI()
{
    constexpr float leftW = 200.f;
    constexpr float rightW = 250.f;
    constexpr float height = 380.f;

    const ImVec2 display = ImGui::GetIO().DisplaySize;

    ImVec2 bottomLeft(leftW, display.y);
    ImVec2 size(display.x - leftW - rightW, height);

    bottomLeft.x = floorf(bottomLeft.x);
    bottomLeft.y = floorf(bottomLeft.y);
    size.x       = floorf(size.x);
    size.y       = floorf(size.y);

    ImGui::SetNextWindowPos(bottomLeft, ImGuiCond_Always, ImVec2(0.f, 1.f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

    ScopedCamToolStyle styleScope;

    if (ImGui::Begin("Camera Tool", nullptr, flags))
    {
        DrawToolbar();
        ImGui::Separator();
        DrawCamSelector();
        ImGui::Separator();

        ImGuiTableFlags layoutFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV;

        if (ImGui::BeginTable("CamToolBodyLayout", 2, layoutFlags, ImVec2(0.f, 0.f)))
        {
            ImVec2 contentAvail = ImGui::GetContentRegionAvail();

            float minRight = 700.f;
            float desiredLeft = max(360.f, contentAvail.x * 0.33f);

            float leftColW = desiredLeft;
            if (contentAvail.x - leftColW < minRight)
                leftColW = max(320.f, contentAvail.x - minRight);

            ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthFixed, leftColW);
            ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::BeginChild("CamToolLeft", ImVec2(0.f, 0.f), true);
            DrawKeyframeList();
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1);
            ImGui::BeginChild("CamToolRight", ImVec2(0.f, 0.f), true);
            DrawKeyframeEditor();
            ImGui::EndChild();

            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void CamPanel::SetCaptureTarget(CamObj* camObj)
{
    target.captureCamObj = camObj;
    target.captureCamComp = camObj->Get_Component<CCamera>();
}

void CamPanel::DrawToolbar()
{
    const ImVec2 buttonSize(80.f, 0.f);

    if (ImGui::Button("Record", buttonSize)) { state.recording = !state.recording; }
    ImGui::SameLine();
    if (ImGui::Button("Play", buttonSize)) { state.playing = true; }
    ImGui::SameLine();
    if (ImGui::Button("Stop", buttonSize)) { state.playing = false; state.curTime = 0.f; }
    ImGui::SameLine();
    ImGui::Checkbox("Loop", &state.loop);

    ImGui::SameLine();
    DrawTimeline();
}

void CamPanel::DrawCamSelector()
{
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Target");
    ImGui::SameLine();

    const char* preview = "None";
    if (target.sequence)
        preview = target.sequence->name.c_str();

    ImGui::SetNextItemWidth(260.f);
    if (ImGui::BeginCombo("##cam_track_combo", preview))
    {
        ImGui::Selectable(preview, true);
        ImGui::EndCombo();
    }
}

void CamPanel::DrawKeyframeArea()
{
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float left = 260.0f; 

    ImGui::BeginChild("KeyList", ImVec2(left, avail.y), true);
    DrawKeyframeList();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("KeyEditor", ImVec2(0, avail.y), true);
    DrawKeyframeEditor();
    ImGui::EndChild();
}

void CamPanel::DrawKeyframeList()
{
    if (!target.sequence)
    {
        ImGui::TextDisabled("No sequence selected.");
        return;
    }

    auto& keys = GetKeys();

    const ImVec2 btnSize(78.f, 0.f);

    if (ImGui::Button("+ Add", btnSize))
        AddKey_Default();

    ImGui::SameLine();

    const bool canDelete = HasValidSelection();
    if (!canDelete) ImGui::BeginDisabled();
    if (ImGui::Button("- Delete", btnSize))
        DeleteSelectedKey();
    if (!canDelete) ImGui::EndDisabled();

    ImGui::SameLine();

    const bool canDuplicate = HasValidSelection();
    if (!canDuplicate) ImGui::BeginDisabled();
    if (ImGui::Button("Duplicate", ImVec2(92.f, 0.f)))
    {
        const CamKeyFrame& src = GetSelectedKey();
        CamKeyFrame dup = src;
        dup.keyId = target.nextKeyId++;
        dup.time  = src.time + policy.defaultStepTime;

        keys.push_back(dup);
        SortKeysByTime_Stable();
        MergeNearDuplicateTimes_KeepLast();
        SelectKeyById(dup.keyId);
        SyncEditorFromSelection();
    }
    if (!canDuplicate) ImGui::EndDisabled();

    ImGui::SameLine();

    char summaryBuf[128];
    sprintf_s(summaryBuf, "Keys: %d  |  End: %.1fs", (int)keys.size(), state.endTime);

    float rightX = ImGui::GetWindowContentRegionMax().x;
    float curX   = ImGui::GetCursorPosX();
    float textW  = ImGui::CalcTextSize(summaryBuf).x;
    if (rightX - textW > curX + 10.f)
        ImGui::SetCursorPosX(rightX - textW);

    ImGui::TextDisabled("%s", summaryBuf);

    ImGui::Separator();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Keyframes");
    ImGui::Separator();

    ImVec2 tableSize = ImGui::GetContentRegionAvail();

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg          |
        ImGuiTableFlags_BordersOuter   |
        ImGuiTableFlags_BordersInnerV  |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_ScrollY;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.f, 3.f));

    if (ImGui::BeginTable("KeyframeTable", 4, tableFlags, tableSize))
    {
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 64.f);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 78.f);
        ImGui::TableSetupColumn("Gap", ImGuiTableColumnFlags_WidthFixed, 66.f);
        ImGui::TableSetupColumn("Go", ImGuiTableColumnFlags_WidthFixed, 36.f);

        ImGui::TableHeadersRow();

        const float rowH = ImGui::GetTextLineHeightWithSpacing();

        ImGuiListClipper clipper;
        clipper.Begin((int)keys.size(), rowH);

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                const CamKeyFrame& key = keys[(size_t)i];
                const bool isSelected = (state.selectedKeyIdx == i);

                ImGui::TableNextRow();
                ImGui::PushID(i);

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable("##row", isSelected, ImGuiSelectableFlags_SpanAllColumns))
                {
                    state.selectedKeyIdx = i;
                    SyncEditorFromSelection();
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    state.curTime = key.time;

                ImGui::SameLine();
                ImGui::Text("#%03u", key.keyId);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.1fs", key.time);

                ImGui::TableSetColumnIndex(2);
                if ((size_t)i + 1 < keys.size())
                    ImGui::Text("%.1fs", keys[(size_t)i + 1].time - key.time);
                else
                    ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(3);
                if (ImGui::SmallButton(">"))
                    state.curTime = key.time;

                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}


void CamPanel::DrawKeyframeEditor()
{
    ImGui::SeparatorText("Selected Key");

    if (!HasValidSelection())
    {
        ImGui::TextDisabled("Select a keyframe from the list.");
        return;
    }

    CamKeyFrame* keyPtr = &GetSelectedKey();

    constexpr float labelWidth    = 70.f;
    constexpr float floatWidth    = 150.f;
    constexpr float vecValueWidth = 80.f;
    constexpr float axisGap       = 10.f;

    auto DrawLabelButton = [&](const char* text)
        {
            char id[128];
            sprintf_s(id, "%s##row_label", text);
            ImGui::Button(id, ImVec2(labelWidth, 0.f));
        };

    auto BeginRow = [&](const char* label)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            DrawLabelButton(label);
            ImGui::TableSetColumnIndex(1);
        };

    auto DrawAxisFloat = [&](const char* axisText, const char* id, float& v, float speed, float minV, float maxV)
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(axisText);
            ImGui::SameLine();

            ImGui::SetNextItemWidth(vecValueWidth);
            if (minV < maxV)
                return ImGui::DragFloat(id, &v, speed, minV, maxV);
            return ImGui::DragFloat(id, &v, speed);
        };

    auto DrawVec3Row_LeftLabel = [&](const char* rowId, _vector3& v, float speed)
        {
            ImGui::PushID(rowId);

            bool changed = false;

            changed |= DrawAxisFloat("X", "##x", v.x, speed, 0.f, 0.f);
            ImGui::SameLine(0.f, axisGap);

            changed |= DrawAxisFloat("Y", "##y", v.y, speed, 0.f, 0.f);
            ImGui::SameLine(0.f, axisGap);

            changed |= DrawAxisFloat("Z", "##z", v.z, speed, 0.f, 0.f);

            ImGui::PopID();
            return changed;
        };

    auto DrawFloatRow = [&](const char* rowId, float& v, float speed, float minV, float maxV)
        {
            ImGui::PushID(rowId);
            ImGui::SetNextItemWidth(floatWidth);
            bool changed = ImGui::DragFloat("##val", &v, speed, minV, maxV);
            ImGui::PopID();
            return changed;
        };

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_PadOuterX      |
        ImGuiTableFlags_RowBg          |
        ImGuiTableFlags_BordersOuter   |
        ImGuiTableFlags_BordersInnerV;

    if (ImGui::BeginTable("KeyEditorTable", 2, tableFlags))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        BeginRow("t:");
        {
            ImGui::PushID("time");
            ImGui::SetNextItemWidth(floatWidth);

            ImGui::DragFloat("##time", &state.editTime, 0.01f, 0.f, 9999.f);
            if (state.editTime < 0.f)
                state.editTime = 0.f;

            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                const _uint selectedId = GetSelectedKeyId();
                ApplyEditorToSelectedKey_TimeOnly();
                SelectKeyById(selectedId);

                if (HasValidSelection())
                    keyPtr = &GetSelectedKey();
            }

            ImGui::SameLine();
            ImGui::TextDisabled("sec");

            ImGui::PopID();
        }

        BeginRow("Pos");
        {
            _vector3 pos = keyPtr->pos;
            if (DrawVec3Row_LeftLabel("pos", pos, 0.05f))
                keyPtr->pos = pos;
        }

        BeginRow("Look");
        {
            _vector3 look = keyPtr->look;
            if (DrawVec3Row_LeftLabel("look", look, 0.01f))
            {
                if (look.LengthSquared() > 1e-8f)
                {
                    look.Normalize();
                    keyPtr->look = look;
                }
            }
        }

        BeginRow("Roll");
        {
            float roll = keyPtr->roll;
            if (DrawFloatRow("roll", roll, 0.01f, -XM_PI, XM_PI))
                keyPtr->roll = roll;
        }

        BeginRow("FOV");
        {
            float fov = keyPtr->fov;
            if (DrawFloatRow("fov", fov, 0.1f, 1.f, 179.f))
                keyPtr->fov = fov;
        }

        BeginRow("Capture");
        {
            const bool hasCaptureTarget = (target.captureCamObj != nullptr);

            if (!hasCaptureTarget)
                ImGui::BeginDisabled();

            ImGui::PushID("capture_action");
            if (ImGui::Button("Capture", ImVec2(140.f, 0.f)))
                CaptureSelectedKey_FromCaptureCam();
            ImGui::PopID();

            if (!hasCaptureTarget)
                ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::TextDisabled(hasCaptureTarget ? "(from DebugFreeCam)" : "(no capture camera)");
        }

        ImGui::EndTable();
    }
}


void CamPanel::DrawTimeline()
{
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float barH = ImGui::GetFrameHeight();

    if (avail.x < 140.f)
        return;

    float endT = (state.endTime > 1e-6f) ? state.endTime : 1.f;

    ImVec2 barPos = ImGui::GetCursorScreenPos();
    ImVec2 barSize(avail.x, barH);

    ImGui::InvisibleButton("##timeline_bar", barSize);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImU32 colBg     = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 colFill   = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    ImU32 colTick   = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colCursor = ImGui::GetColorU32(ImGuiCol_Text);
    ImU32 colText   = ImGui::GetColorU32(ImGuiCol_TextDisabled);

    dl->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), colBg, 4.f);
    dl->AddRect(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), colBorder, 4.f);

    float t01 = state.curTime / endT;
    if (t01 < 0.f) t01 = 0.f;
    if (t01 > 1.f) t01 = 1.f;

    dl->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x * t01, barPos.y + barSize.y), colFill, 4.f);

    if (target.sequence)
    {
        const auto& keys = GetKeys();
        for (size_t i = 0; i < keys.size(); ++i)
        {
            float k01 = keys[i].time / endT;
            if (k01 < 0.f) k01 = 0.f;
            if (k01 > 1.f) k01 = 1.f;

            float x = barPos.x + barSize.x * k01;
            dl->AddLine(ImVec2(x, barPos.y + 2.f), ImVec2(x, barPos.y + barSize.y - 2.f), colTick, 1.0f);
        }
    }

    float cx = barPos.x + barSize.x * t01;
    dl->AddLine(ImVec2(cx, barPos.y - 2.f), ImVec2(cx, barPos.y + barSize.y + 2.f), colCursor, 2.0f);

    char buf[64];
    sprintf_s(buf, "%.2fs / %.2fs", state.curTime, state.endTime);

    ImVec2 textSize = ImGui::CalcTextSize(buf);
    ImVec2 textPos(barPos.x + 8.f, barPos.y + (barSize.y - textSize.y) * 0.5f);
    dl->AddText(textPos, colText, buf);

    if (ImGui::IsItemActive())
    {
        float mx = ImGui::GetIO().MousePos.x;
        float local = (mx - barPos.x) / barSize.x;
        if (local < 0.f) local = 0.f;
        if (local > 1.f) local = 1.f;
        state.curTime = local * endT;
    }
}

vector<CamKeyFrame>& CamPanel::GetKeys()
{
    assert(target.sequence);
    return target.sequence->keyframes;
}

const vector<CamKeyFrame>& CamPanel::GetKeys() const
{
    assert(target.sequence);
    return target.sequence->keyframes;
}

bool CamPanel::HasValidSelection() const
{
    if (!target.sequence) return false;
    if (state.selectedKeyIdx < 0) return false;
    if (state.selectedKeyIdx >= (_int)target.sequence->keyframes.size()) return false;
    return true;
}

CamKeyFrame& CamPanel::GetSelectedKey()
{
    assert(HasValidSelection());
    return target.sequence->keyframes[(size_t)state.selectedKeyIdx];
}

_uint CamPanel::GetSelectedKeyId() const
{
    assert(HasValidSelection());
    return target.sequence->keyframes[(size_t)state.selectedKeyIdx].keyId;
}

_float CamPanel::GetNextDefaultTime() const
{
    if (!target.sequence || target.sequence->keyframes.empty())
        return 0.f;

    const _float lastTime = target.sequence->keyframes.back().time;
    return lastTime + policy.defaultStepTime;
}

void CamPanel::AddKey_Default()
{
    assert(target.sequence);

    CamKeyFrame newKey{};
    newKey.keyId = target.nextKeyId++;
    newKey.time = GetNextDefaultTime();

    auto& keys = GetKeys();

    if (!keys.empty())
    {
        const CamKeyFrame& lastKey = keys.back();
        newKey.pos  = lastKey.pos;
        newKey.look = lastKey.look;
        newKey.roll = lastKey.roll;
        newKey.fov  = lastKey.fov;
    }
    else
    {
        newKey.look = _vector3{ 0.f, 0.f, 1.f };
        newKey.fov  = state.editFov;   
        newKey.roll = 0.f;
    }
    keys.push_back(newKey);

    SortKeysByTime_Stable();
    MergeNearDuplicateTimes_KeepLast();
    SelectKeyById(newKey.keyId);
    SyncEditorFromSelection();
}

void CamPanel::DeleteSelectedKey()
{
    if (!HasValidSelection()) return;

    auto& keys = GetKeys();
    assert(!keys.empty());

    const _int deleteIdx = state.selectedKeyIdx;
    keys.erase(keys.begin() + deleteIdx);

    if (keys.empty())
    {
        state.selectedKeyIdx = -1;
        return;
    }
    state.selectedKeyIdx = clamp(deleteIdx, 0, (_int)keys.size() - 1);
    SyncEditorFromSelection();
}

void CamPanel::SortKeysByTime_Stable()
{
    auto& keys = GetKeys();
    stable_sort(keys.begin(), keys.end(), [](const CamKeyFrame& a, const CamKeyFrame& b) { return a.time < b.time; });
}

void CamPanel::MergeNearDuplicateTimes_KeepLast()
{
    auto& keys = GetKeys();
    if (keys.size() < 2) return;

    vector<CamKeyFrame> merged;
    merged.reserve(keys.size());

    for (size_t idx = 0; idx < keys.size(); ++idx)
    {
        const CamKeyFrame& cur = keys[idx];

        if (merged.empty())
        {
            merged.push_back(cur);
            continue;
        }

        CamKeyFrame& last = merged.back();

        if (fabsf(cur.time - last.time) <= policy.mergeEpsilon)
            last = cur;
        else
            merged.push_back(cur);
    }
    keys.swap(merged);
}

bool CamPanel::SelectKeyById(_uint keyId)
{
    if (!target.sequence)
    {
        state.selectedKeyIdx = -1;
        return false;
    }

    auto& keys = GetKeys();
    for (size_t idx = 0; idx < keys.size(); ++idx)
    {
        if (keys[idx].keyId == keyId)
        {
            state.selectedKeyIdx = (_int)idx;
            return true;
        }
    }
    state.selectedKeyIdx = -1;
    return false;
}

void CamPanel::SyncEditorFromSelection()
{
    if (!HasValidSelection())
        return;

    const CamKeyFrame& key = GetSelectedKey();

    state.editTime = key.time;
    state.editFov  = key.fov;
    state.editRoll = key.roll;

    state.curTime = key.time;

    if (state.endTime < key.time)
        state.endTime = key.time;
}

void CamPanel::ApplyEditorToSelectedKey_TimeOnly()
{
    if (!HasValidSelection()) return;

    CamKeyFrame& key = GetSelectedKey();
    const _uint selectedId = key.keyId;

    key.time = max(0.f, state.editTime);

    SortKeysByTime_Stable();
    MergeNearDuplicateTimes_KeepLast();
    SelectKeyById(selectedId);
}

void CamPanel::CaptureSelectedKey_FromCaptureCam()
{
    if (!HasValidSelection()) return;
    if (!target.captureCamObj) return;

    CamKeyFrame& key = GetSelectedKey();

    key.pos = target.captureCamObj->Get_Component<CTransform>()->Get_Pos();
    _vector4 look4 = target.captureCamObj->Get_Component<CTransform>()->Dir(STATE::LOOK);
    _vector3 look3 = { look4.x, look4.y, look4.z };
    look3.Normalize();
    key.look = look3;

    if (target.captureCamComp)
        key.fov = target.captureCamComp->Get_FOV();

    key.roll = 0.f;

    SyncEditorFromSelection();
}

void CamPanel::ApplyPlaybackAtTime(_float t)
{
    if (!target.sequence)
        return;

    const auto& keys = GetKeys();
    if (keys.empty())
        return;

    CamKeyFrame sampled = keys.front();

    if (keys.size() == 1)
    {
        sampled = keys.front();
    }
    else if (t <= keys.front().time)
    {
        sampled = keys.front();
    }
    else if (t >= keys.back().time)
    {
        sampled = keys.back();
    }
    else
    {
        const auto it = std::upper_bound(
            keys.begin(),
            keys.end(),
            t,
            [](float value, const CamKeyFrame& kf) { return value < kf.time; });

        const size_t i1 = (size_t)(it - keys.begin());
        const size_t i0 = i1 - 1;

        const CamKeyFrame& a = keys[i0];
        const CamKeyFrame& b = keys[i1];

        const float seg = b.time - a.time;
        float alpha = (seg > 1e-6f) ? ((t - a.time) / seg) : 0.f;
        alpha = std::clamp(alpha, 0.f, 1.f);

        sampled.pos = a.pos + (b.pos - a.pos) * alpha;

        _vector3 look = a.look + (b.look - a.look) * alpha;
        if (look.LengthSquared() > 1e-8f)
            look.Normalize();
        else
            look = _vector3(0.f, 0.f, 1.f);

        sampled.look = look;
        sampled.roll = a.roll + (b.roll - a.roll) * alpha;
        sampled.fov = a.fov + (b.fov - a.fov) * alpha;
    }

    CamObj* camObj = target.captureCamObj;
    if (!camObj)
        return;

    CTransform* tr = camObj->Get_Component<CTransform>();
    assert(tr);

    tr->Set_Pos(sampled.pos);
    tr->LookAt(sampled.pos + sampled.look);

    if (target.captureCamComp)
        target.captureCamComp->Set_FOV(sampled.fov);

    state.editRoll = sampled.roll;
}

CamPanel* CamPanel::Create(GUI_CONTEXT* context)
{
    auto inst = new CamPanel(context);
    inst->Init();
    return inst;
}

void CamPanel::Free()
{
    __super::Free();
}