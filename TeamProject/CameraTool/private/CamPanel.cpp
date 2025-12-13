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

        if (target.player)
            target.player->SetApplyEnabled(false);

        return;
    }

    const auto& keys = GetKeyFrames();

    float maxT = 0.f;
    for (size_t i = 0; i < keys.size(); ++i)
        maxT = max(maxT, keys[i].time);

    state.endTime = maxT;

    if (state.curTime < 0.f)           state.curTime = 0.f;
    if (state.curTime > state.endTime) state.curTime = state.endTime;

    if (state.timeScale < 0.f)
        state.timeScale = 0.f;

    if (target.player)
    {
        if (target.player->GetSequence() != target.sequence)
            target.player->SetSequence(target.sequence);

        target.player->SetApplyEnabled(!state.recording);
        target.player->SetTimeScale(state.timeScale);
    }

    if (state.playing)
    {
        if (state.endTime <= 1e-6f)
        {
            state.curTime = 0.f;
            state.playing = false;
        }
        else
        {
            state.curTime += dt * state.timeScale;

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

    if (target.player && !state.recording)
        target.player->SetTime(state.curTime);
}


void CamPanel::Render_GUI()
{
    constexpr float leftW  = 200.f;
    constexpr float rightW = 250.f;
    constexpr float height = 400.f;

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
    target.captureCamObj  = camObj;
    target.captureCamComp = camObj ? camObj->Get_Component<CCamera>() : nullptr;
    target.player         = camObj ? camObj->Get_Component<CamSequencePlayer>() : nullptr;

    if (target.player && target.sequence)
        target.player->SetSequence(target.sequence);

    if (target.player)
        target.player->SetApplyEnabled(!state.recording);

    if (target.player && !state.recording)
        target.player->SetTime(state.curTime);
}

void CamPanel::DrawToolbar()
{
    const ImVec2 buttonSize(80.f, 0.f);
    const bool wasRecording = state.recording;
    bool nextRecording = state.recording;

    if (wasRecording)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.35f, 0.10f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.45f, 0.12f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.55f, 0.14f, 0.14f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 0.85f, 0.85f, 1.0f));
    }

    const bool changed = ImGui::Checkbox("CAPTURE", &nextRecording);

    if (wasRecording)
        ImGui::PopStyleColor(5);

    if (changed)
    {
        if (state.recording && !nextRecording)
        {
            const int keyCount = target.sequence ? (int)target.sequence->keyframes.size() : 0;

            if (keyCount < 2)
            {
                ImGui::OpenPopup("CaptureOff_Confirm_TooFewKeys");
                nextRecording = true;
            }
            else
                state.recording = false;
        }
        else
            state.recording = nextRecording;
    }

    if (ImGui::BeginPopupModal("CaptureOff_Confirm_TooFewKeys", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(u8"키프레임이 1개 이하입니다.");
        ImGui::Separator();
        ImGui::TextUnformatted(u8"CAPTURE(REC)를 끄면 재생이 고정처럼 보일 수 있어요.");
        ImGui::TextUnformatted(u8"그래도 끌까요?");

        ImGui::Separator();

        if (ImGui::Button(u8"끄기", ImVec2(120.f, 0.f)))
        {
            state.recording = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"계속 캡처", ImVec2(120.f, 0.f)))
        {
            state.recording = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (state.recording && !wasRecording)
        state.playing = false;

    if (target.player && state.recording != wasRecording)
    {
        target.player->SetApplyEnabled(!state.recording);
        if (!state.recording)
            target.player->SetTime(state.curTime);
    }

    if (state.recording)
    {
        ImGui::SameLine();
        ImGui::TextUnformatted("REC");
    }

    ImGui::SameLine();

    if (state.recording)
        ImGui::BeginDisabled();

    if (ImGui::Button("Play", buttonSize))
    {
        const float eps = 1e-4f;

        state.recording = false;

        if (state.endTime > 1e-6f && state.curTime >= state.endTime - eps)
            state.curTime = 0.f;

        state.playing = true;

        if (target.player)
        {
            target.player->SetApplyEnabled(true);
            target.player->SetTime(state.curTime);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop", buttonSize))
    {
        state.playing = false;

        if (target.player && !state.recording)
            target.player->SetTime(state.curTime);
    }

    if (state.recording)
        ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::Checkbox("Loop", &state.loop);

    ImGui::SameLine();
    ImGui::TextUnformatted(u8"속도");

    ImGui::SameLine();
    {
        const float prevScale = state.timeScale;

        ImGui::PushID("TimeScaleUI");

        if (ImGui::SmallButton(u8"x0.25")) state.timeScale = 0.25f;
        ImGui::SameLine();
        if (ImGui::SmallButton(u8"x0.5"))  state.timeScale = 0.5f;
        ImGui::SameLine();
        if (ImGui::SmallButton(u8"x1"))    state.timeScale = 1.0f;
        ImGui::SameLine();
        if (ImGui::SmallButton(u8"x2"))    state.timeScale = 2.0f;
        ImGui::SameLine();
        if (ImGui::SmallButton(u8"x4"))    state.timeScale = 4.0f;

        ImGui::SameLine();
        ImGui::SetNextItemWidth(90.f);
        ImGui::DragFloat("##scale", &state.timeScale, 0.01f, 0.05f, 8.0f, "x%.2f");

        if (state.timeScale < 0.05f) state.timeScale = 0.05f;

        if (prevScale != state.timeScale)
        {
            if (target.player)
                target.player->SetTimeScale(state.timeScale);
        }

        ImGui::PopID();
    }

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

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(14.f, 0.f));
    ImGui::SameLine();

    if (!target.sequence)
    {
        ImGui::TextDisabled("(No Sequence)");
        return;
    }

    bool changedAny = false;

    ImGui::PushID("InterpInline");

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Pos");
    ImGui::SameLine();
    {
        const char* posPreview = "Linear";
        if (target.sequence->posInterp == CamPosInterp::CatmullRom) posPreview = "CatmullRom";
        if (target.sequence->posInterp == CamPosInterp::Centripetal) posPreview = "Centripetal";

        ImGui::SetNextItemWidth(140.f);

        if (ImGui::BeginCombo("##pos_interp", posPreview))
        {
            if (ImGui::Selectable("Linear", target.sequence->posInterp == CamPosInterp::Linear))
            {
                target.sequence->posInterp = CamPosInterp::Linear;
                changedAny = true;
            }
            if (ImGui::Selectable("CatmullRom", target.sequence->posInterp == CamPosInterp::CatmullRom))
            {
                target.sequence->posInterp = CamPosInterp::CatmullRom;
                changedAny = true;
            }
            if (ImGui::Selectable("Centripetal", target.sequence->posInterp == CamPosInterp::Centripetal))
            {
                target.sequence->posInterp = CamPosInterp::Centripetal;
                changedAny = true;
            }
            ImGui::EndCombo();
        }
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Rot");
    ImGui::SameLine();
    {
        const char* rotPreview = (target.sequence->rotInterp == CamRotInterp::Slerp) ? "Slerp" : "Squad";
        ImGui::SetNextItemWidth(140.f);

        if (ImGui::BeginCombo("##rot_interp", rotPreview))
        {
            if (ImGui::Selectable("Slerp", target.sequence->rotInterp == CamRotInterp::Slerp))
            {
                target.sequence->rotInterp = CamRotInterp::Slerp;
                changedAny = true;
            }

            ImGui::BeginDisabled();
            ImGui::Selectable("Squad (WIP)", target.sequence->rotInterp == CamRotInterp::Squad);
            ImGui::EndDisabled();

            ImGui::EndCombo();
        }
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("FOV");
    ImGui::SameLine();
    {
        const char* fovPreview = (target.sequence->fovInterp == CamFovInterp::Linear) ? "Linear" : "Smooth";
        ImGui::SetNextItemWidth(140.f);

        if (ImGui::BeginCombo("##fov_interp", fovPreview))
        {
            if (ImGui::Selectable("Linear", target.sequence->fovInterp == CamFovInterp::Linear))
            {
                target.sequence->fovInterp = CamFovInterp::Linear;
                changedAny = true;
            }
            if (ImGui::Selectable("Smooth", target.sequence->fovInterp == CamFovInterp::Smooth))
            {
                target.sequence->fovInterp = CamFovInterp::Smooth;
                changedAny = true;
            }
            ImGui::EndCombo();
        }
    }

    ImGui::PopID();

    if (changedAny && target.player)
    {
        target.player->Invalidate();
        if (!state.recording)
            target.player->SetTime(state.curTime);
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

    auto& keys = GetKeyFrames();
    const ImVec2 btnSize(78.f, 0.f);

    if (ImGui::Button("+ Add", btnSize))
    {
        if (state.recording)
            AddKey_Default();
        else
            ImGui::OpenPopup("AddKey_Confirm_NotCapture");
    }

    if (ImGui::BeginPopupModal("AddKey_Confirm_NotCapture", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(u8"CAPTURE(REC) OFF 상태에서 키를 추가할까요?");
        ImGui::Separator();
        ImGui::TextUnformatted(u8"- 이 키는 카메라에서 캡쳐되지 않습니다.");
        ImGui::TextUnformatted(u8"- 마지막 키 복사/기본값으로 생성되며, 이후 Capture로 덮어쓸 수 있습니다.");

        ImGui::Separator();

        if (ImGui::Button(u8"추가", ImVec2(120.f, 0.f)))
        {
            AddKey_Default();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"취소", ImVec2(120.f, 0.f)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

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
        dup.time = src.time + policy.defaultStepTime;

        keys.push_back(dup);
        SortKeysByTime_Stable();
        MergeNearDuplicateTimes_KeepLast();
        SelectKeyById(dup.keyId);
        SyncEditorFromSelection();

        if (target.player)
            target.player->Invalidate();
    }
    if (!canDuplicate) ImGui::EndDisabled();

    ImGui::SameLine();

    char summaryBuf[128];
    sprintf_s(summaryBuf, "Keys: %d  |  End: %.1fs", (int)keys.size(), state.endTime);

    float rightX = ImGui::GetWindowContentRegionMax().x;
    float curX = ImGui::GetCursorPosX();
    float textW = ImGui::CalcTextSize(summaryBuf).x;
    if (rightX - textW > curX + 10.f)
        ImGui::SetCursorPosX(rightX - textW);

    ImGui::TextDisabled("%s", summaryBuf);

    ImGui::Separator();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Keyframes");
    ImGui::Separator();

    ImVec2 tableSize = ImGui::GetContentRegionAvail();

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_BordersInnerV |
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

        BeginRow("Time");
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

    ImDrawList* dl  = ImGui::GetWindowDrawList();

    ImU32 colBg     = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 colFill   = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    ImU32 colTick   = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colCursor = ImGui::GetColorU32(ImGuiCol_Text);
    ImU32 colText   = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colHot    = ImGui::GetColorU32(ImGuiCol_ButtonActive);

    dl->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), colBg, 4.f);
    dl->AddRect(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), colBorder, 4.f);

    float t01 = clamp(state.curTime / endT, 0.f, 1.f);
    dl->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x * t01, barPos.y + barSize.y), colFill, 4.f);

    int hotIndex = -1;
    float bestDx = FLT_MAX;

    const bool hovered = ImGui::IsItemHovered();
    float hoverTime = 0.f;

    if (hovered && target.sequence)
    {
        const float mx = ImGui::GetIO().MousePos.x;
        const float local01 = clamp((mx - barPos.x) / barSize.x, 0.f, 1.f);
        hoverTime = local01 * endT;

        const auto& keys = GetKeyFrames();
        for (int i = 0; i < (int)keys.size(); ++i)
        {
            float k01 = clamp(keys[(size_t)i].time / endT, 0.f, 1.f);
            float x = barPos.x + barSize.x * k01;
            float dx = fabsf(mx - x);

            if (dx < bestDx)
            {
                bestDx = dx;
                hotIndex = i;
            }
        }
    }

    if (target.sequence)
    {
        const auto& keys = GetKeyFrames();
        const float padY = 2.f;

        const bool hasSel = HasValidSelection();
        const _uint selId = hasSel ? GetSelectedKeyId() : 0;

        const float snapPx = 7.f;
        const bool hotActive = hovered && (hotIndex >= 0) && (bestDx <= snapPx);

        for (int i = 0; i < (int)keys.size(); ++i)
        {
            const CamKeyFrame& k = keys[(size_t)i];

            float k01 = std::clamp(k.time / endT, 0.f, 1.f);
            float x = barPos.x + barSize.x * k01;

            const bool isSelected = hasSel && (k.keyId == selId);
            const bool isHot = hotActive && (i == hotIndex);

            ImU32 lineCol = isHot ? colHot : (isSelected ? colCursor : colTick);
            float thick = isHot ? 2.5f : (isSelected ? 2.0f : 1.0f);

            dl->AddLine(ImVec2(x, barPos.y + padY), ImVec2(x, barPos.y + barSize.y - padY), lineCol, thick);

            float triH = isHot ? 7.f : (isSelected ? 6.f : 5.f);
            float triW = isHot ? 6.f : (isSelected ? 5.f : 4.f);

            ImVec2 p0(x, barPos.y - 1.f);
            ImVec2 p1(x - triW, barPos.y - 1.f + triH);
            ImVec2 p2(x + triW, barPos.y - 1.f + triH);

            dl->AddTriangleFilled(p0, p1, p2, lineCol);
        }
    }

    float cx = barPos.x + barSize.x * t01;
    dl->AddLine(ImVec2(cx, barPos.y - 2.f), ImVec2(cx, barPos.y + barSize.y + 2.f), colCursor, 2.0f);

    ImVec2 playTri0(cx, barPos.y + barSize.y + 1.f);
    ImVec2 playTri1(cx - 5.f, barPos.y + barSize.y + 9.f);
    ImVec2 playTri2(cx + 5.f, barPos.y + barSize.y + 9.f);
    dl->AddTriangleFilled(playTri0, playTri1, playTri2, colCursor);

    char buf[64];
    sprintf_s(buf, "%.2fs / %.2fs", state.curTime, state.endTime);

    ImVec2 textSize = ImGui::CalcTextSize(buf);
    ImVec2 textPos(barPos.x + 8.f, barPos.y + (barSize.y - textSize.y) * 0.5f);
    dl->AddText(textPos, colText, buf);

    if (hovered && target.sequence)
    {
        const auto& keys = GetKeyFrames();

        const CamKeyFrame* nearest = nullptr;
        if (hotIndex >= 0 && hotIndex < (int)keys.size())
            nearest = &keys[(size_t)hotIndex];

        ImGui::BeginTooltip();
        ImGui::Text("t = %.3fs", hoverTime);

        const float snapPx = 7.f;
        if (nearest && bestDx <= snapPx)
        {
            ImGui::Separator();
            ImGui::Text("Key #%03u", nearest->keyId);
            ImGui::Text("Time : %.3fs", nearest->time);
            ImGui::Text("Pos  : %.3f, %.3f, %.3f", nearest->pos.x, nearest->pos.y, nearest->pos.z);
            ImGui::Text("Look : %.3f, %.3f, %.3f", nearest->look.x, nearest->look.y, nearest->look.z);
            ImGui::Text("Roll : %.3f", nearest->roll);
            ImGui::Text("FOV  : %.3f", nearest->fov);
        }
        ImGui::EndTooltip();
    }

    if (ImGui::IsItemActive())
    {
        float mx = ImGui::GetIO().MousePos.x;
        float local = std::clamp((mx - barPos.x) / barSize.x, 0.f, 1.f);
        state.curTime = local * endT;

        if (target.player && !state.recording)
            target.player->SetTime(state.curTime);
    }
}

void CamPanel::DrawInterpSelector()
{
    if (!target.sequence)
        return;

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(u8"보간");
    ImGui::SameLine();

    ImGui::PushID("InterpSelector");

    const char* posLabels[] = { "Linear", "CatmullRom" };
    int posIdx = (target.sequence->posInterp == CamPosInterp::Linear) ? 0 : 1;

    ImGui::SetNextItemWidth(140.f);
    if (ImGui::Combo("Pos", &posIdx, posLabels, IM_ARRAYSIZE(posLabels)))
    {
        target.sequence->posInterp = (posIdx == 0) ? CamPosInterp::Linear : CamPosInterp::CatmullRom;

        if (target.player)
        {
            target.player->Invalidate();
            if (!state.recording)
                target.player->SetTime(state.curTime);
        }
    }

    ImGui::SameLine();

    const char* fovLabels[] = { "Linear", "Smooth" };
    int fovIdx = (target.sequence->fovInterp == CamFovInterp::Linear) ? 0 : 1;

    ImGui::SetNextItemWidth(140.f);
    if (ImGui::Combo("FOV", &fovIdx, fovLabels, IM_ARRAYSIZE(fovLabels)))
    {
        target.sequence->fovInterp = (fovIdx == 0) ? CamFovInterp::Linear : CamFovInterp::Smooth;

        if (target.player)
        {
            target.player->Invalidate();
            if (!state.recording)
                target.player->SetTime(state.curTime);
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled(u8"(Rot: Slerp)");
    ImGui::PopID();
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

    auto& keys = GetKeyFrames();

    if (!keys.empty())
    {
        const CamKeyFrame& lastKey = keys.back();
        newKey.pos = lastKey.pos;
        newKey.look = lastKey.look;
        newKey.roll = lastKey.roll;
        newKey.fov = lastKey.fov;
    }
    else
    {
        newKey.look = _vector3{ 0.f, 0.f, 1.f };
        newKey.fov = state.editFov;
        newKey.roll = 0.f;
    }

    keys.push_back(newKey);

    SortKeysByTime_Stable();
    MergeNearDuplicateTimes_KeepLast();
    SelectKeyById(newKey.keyId);
    SyncEditorFromSelection();

    if (target.player)
        target.player->Invalidate();
}

void CamPanel::DeleteSelectedKey()
{
    if (!HasValidSelection()) return;

    auto& keys = GetKeyFrames();
    assert(!keys.empty());

    const _int deleteIdx = state.selectedKeyIdx;
    keys.erase(keys.begin() + deleteIdx);

    if (keys.empty())
    {
        state.selectedKeyIdx = -1;

        if (target.player)
            target.player->Invalidate();

        return;
    }

    state.selectedKeyIdx = clamp(deleteIdx, 0, (_int)keys.size() - 1);
    SyncEditorFromSelection();

    if (target.player)
        target.player->Invalidate();
}

void CamPanel::SortKeysByTime_Stable()
{
    auto& keys = GetKeyFrames();
    stable_sort(keys.begin(), keys.end(), [](const CamKeyFrame& a, const CamKeyFrame& b) { return a.time < b.time; });
}

void CamPanel::MergeNearDuplicateTimes_KeepLast()
{
    auto& keys = GetKeyFrames();
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

    auto& keys = GetKeyFrames();
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
    if (!HasValidSelection()) return;

    const CamKeyFrame& key = GetSelectedKey();

    state.editTime = key.time;
    state.editFov  = key.fov;
    state.editRoll = key.roll;

    state.curTime = key.time;

    if (state.endTime < key.time)
        state.endTime = key.time;

    if (target.player && !state.recording)
        target.player->SetTime(state.curTime);
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

    if (target.player)
    {
        target.player->Invalidate();
        if (!state.recording)
            target.player->SetTime(state.curTime);
    }
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

    if (target.player)
        target.player->Invalidate();
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