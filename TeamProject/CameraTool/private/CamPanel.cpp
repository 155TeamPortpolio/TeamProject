#include "pch.h"
#include "CamPanel.h"

#include "CamPanelUtil.h"
#include "GUIUtil.h"
#include "Helper_Func.h"

using namespace CamPanelUtil;
using namespace GuiUtil;
using namespace Helper;
namespace fs = filesystem;

namespace
{
    filesystem::path GetCamToolSettingsPath()
    {
        return filesystem::path("..") / "bin" / "Resources" / "Data" / "Camera" / "_CamToolSettings.txt";
    }

    string NormalizeCamPathForLoad(const string& raw)
    {
        if (raw.empty()) return {};

        filesystem::path p = filesystem::path(raw);

        if (p.extension().string() != ".cam")
            p += ".cam";

        filesystem::path abs = filesystem::absolute(p).lexically_normal();
        return abs.string();
    }
    Matrix GetRefRT(OBJECT_HANDLE h)
    {
        auto obj = OBJ->Request_Object(h);
        auto tr = obj->Get_Component<CTransform>();

        Matrix refWorld = Matrix(tr->Get_WorldMatrix());

        Vector3 s{};
        Vector3 t{};
        Quaternion r = Quaternion::Identity;
        refWorld.Decompose(s, r, t);
        r.Normalize();

        return Matrix::CreateFromQuaternion(r) * Matrix::CreateTranslation(t);
    }
    _vector3 ToLocalPos(_vector3 worldPos, const Matrix& refRT)
    {
        const Matrix inv = refRT.Invert();
        const Vector3 p = Vector3::Transform(Vector3(worldPos.x, worldPos.y, worldPos.z), inv);
        return _vector3(p.x, p.y, p.z);
    }
    _vector3 ToLocalDir(_vector3 worldDir, const Matrix& refRT)
    {
        const Matrix inv = refRT.Invert();

        Vector3 d = Vector3::TransformNormal(Vector3(worldDir.x, worldDir.y, worldDir.z), inv);
        d.Normalize();
        return _vector3(d.x, d.y, d.z);
    }
    _vector3 ToWorldPos(_vector3 localPos, const Matrix& refRT)
    {
        const Vector3 p = Vector3::Transform(Vector3(localPos.x, localPos.y, localPos.z), refRT);
        return _vector3(p.x, p.y, p.z);
    }
    _vector3 ToWorldDir(_vector3 localDir, const Matrix& refRT)
    {
        Vector3 d = Vector3::TransformNormal(Vector3(localDir.x, localDir.y, localDir.z), refRT);
        d.Normalize();
        return _vector3(d.x, d.y, d.z);
    }
    void ConvertKeysSpace(vector<CamKeyFrame>& keys, CamSpace from, CamSpace to, OBJECT_HANDLE refHandle)
    {
        if (from == to) return;

        const Matrix refRT = GetRefRT(refHandle);

        if (from == CamSpace::World && to == CamSpace::Local)
        {
            for (auto& k : keys)
            {
                k.pos = ToLocalPos(k.pos, refRT);
                k.look = ToLocalDir(k.look, refRT);
            }
            return;
        }
        if (from == CamSpace::Local && to == CamSpace::World)
        {
            for (auto& k : keys)
            {
                k.pos = ToWorldPos(k.pos, refRT);
                k.look = ToWorldDir(k.look, refRT);
            }
        }
    }
    float Approach(float cur, float target, float maxDelta)
    {
        if (cur < target) return min(cur + maxDelta, target);
        return max(cur - maxDelta, target);
    }
}

void CCamPanel::Init()
{
    debugSequence.name = "DebugSequence";
    debugSequence.projType = CamProjType::Perspective;
    debugSequence.playbackMode = CamPlaybackMode::Once;

    target.sequence = &debugSequence;
    state.recording = true;
    state.playing = false;

    LoadToolSettings();
    AutoLoadSequenceIfSet();
}

void CCamPanel::Update_Panel(_float dt)
{
    if (!target.sequence)
    {
        state.selectedKeyIdx = -1;
        state.endTime = 0.f;
        state.curTime = 0.f;
        state.playing = false;

        state.playAllLink = false;
        state.playAllRefHandle = {};

        if (target.player)
            target.player->SetApplyEnabled(false);

        return;
    }

    target.sequence->orbitArc.NormalizeAxis();

    RecalcEndTimeFromKeys();
    ClampCurTime();

    if (state.timeScale < 0.f)
        state.timeScale = 0.f;

    if (target.player)
    {
        if (target.player->GetSequence() != target.sequence)
            target.player->SetSequence(target.sequence);

        if (target.sequence->space == CamSpace::Local)
            target.player->SetSpaceReference(target.spaceRefHandle);
        else
            target.player->ClearSpaceReference();

        target.player->SetApplyEnabled(!state.recording);
        target.player->SetTimeScale(state.timeScale);
    }

    if (target.captureCamObj)
        target.captureCamObj->SetControlEnabled(state.recording);

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

    if (state.playAllLink && !state.recording)
        animGUIController.SetTimeSec(state.playAllRefHandle, state.curTime);
}


void CCamPanel::Render_GUI()
{
    constexpr float leftW = 200.f;
    constexpr float rightW = 250.f;

    constexpr float expandedMinH = 220.f;

    const ImVec2 display = ImGui::GetIO().DisplaySize;

    float desiredW = display.x - leftW - rightW;
    desiredW = floorf(desiredW);
    if (desiredW < 360.f) desiredW = 360.f;

    float maxH = floorf(display.y - 60.f);
    if (maxH < expandedMinH) maxH = expandedMinH;

    const float hideExtra = 40.f;
    const float hideTarget = panelUI.hidden ? (panelUI.expandedH + hideExtra) : 0.f;

    const float dt = ImGui::GetIO().DeltaTime;
    const float slideSpeed = 900.f;
    panelUI.slideY = Approach(panelUI.slideY, hideTarget, slideSpeed * dt);

    ImVec2 bottomLeft(leftW, display.y + panelUI.slideY);
    bottomLeft.x = floorf(bottomLeft.x);
    bottomLeft.y = floorf(bottomLeft.y);

    ImGui::SetNextWindowPos(bottomLeft, ImGuiCond_Always, ImVec2(0.f, 1.f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(desiredW, expandedMinH), ImVec2(desiredW, maxH));
    ImGui::SetNextWindowSize(ImVec2(desiredW, panelUI.expandedH), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar;

    Helper::DarkThemeStyle styleScope;

    const bool began = ImGui::Begin("Camera Tool##CamToolWindow", nullptr, flags);

    if (began)
    {
        const ImVec2 curSize = ImGui::GetWindowSize();
        if (fabsf(curSize.x - desiredW) > 0.5f)
            ImGui::SetWindowSize(ImVec2(desiredW, curSize.y));

        if (!panelUI.hidden)
            panelUI.expandedH = clamp(curSize.y, expandedMinH, maxH);

        DrawWindowHeader();

        if (!panelUI.hidden)
        {
            DrawToolbar();
            ImGui::Separator();

            DrawCamSelector();
            ImGui::Separator();

            ImGuiTableFlags layoutFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV;

            if (ImGui::BeginTable("CamToolBodyLayout", 2, layoutFlags, ImVec2(0.f, 0.f)))
            {
                ImVec2 contentAvail = ImGui::GetContentRegionAvail();

                float minRight = 420.f;
                float minLeft = 520.f;

                float desiredLeft = contentAvail.x * 0.60f;
                float maxLeft = contentAvail.x - minRight;

                float leftColW = desiredLeft;
                leftColW = clamp(leftColW, minLeft, maxLeft);
                leftColW = max(leftColW, 360.f);

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
    }

    ImGui::End();

    if (panelUI.hidden)
        DrawHiddenHandle();

    DrawPath();
}

void CCamPanel::SetCaptureTarget(CCamObj* camObj)
{
    target.captureCamObj = camObj;
    target.captureCamComp = camObj ? camObj->Get_Component<CCamera>() : nullptr;
    target.player = camObj ? camObj->Get_Component<CCamSequencePlayer>() : nullptr;

    if (target.player && target.sequence)
        target.player->SetSequence(target.sequence);

    if (target.player)
    {
        target.player->SetSpaceReference(target.spaceRefHandle);
        target.player->SetApplyEnabled(!state.recording);
        target.player->SetTimeScale(state.timeScale);

        if (!state.recording)
            target.player->SetTime(state.curTime);
    }

    if (target.captureCamObj)
        target.captureCamObj->SetControlEnabled(state.recording);
}

void CCamPanel::DrawToolbar()
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
                SetRecording(false);
        }
        else
            SetRecording(nextRecording);
    }

    const ConfirmResult capOff = DrawConfirmPopupModal( "CaptureOff_Confirm_TooFewKeys", nullptr, { u8"키프레임이 1개 이하입니다.", u8"CAPTURE(REC)를 끄면 재생이 고정처럼 보일 수 있어요.", u8"그래도 끌까요?" }, u8"끄기", u8"계속 캡처", 120.f);

    if (capOff == ConfirmResult::Ok)
        SetRecording(false);
    else if (capOff == ConfirmResult::Cancel)
        SetRecording(true);

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

        SetRecording(false);

        if (state.endTime > 1e-6f && state.curTime >= state.endTime - eps)
            state.curTime = 0.f;

        SetPlaying(true);
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop", buttonSize))
        SetPlaying(false);

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

        if (prevScale != state.timeScale && target.player)
            target.player->SetTimeScale(state.timeScale);

        ImGui::PopID();
    }

    ImGui::SameLine();
    DrawTimeline();
}

void CCamPanel::DrawCamSelector()
{
    const bool hasSeq = (target.sequence != nullptr);

    if (!hasSeq)
    {
        ImGui::TextDisabled("(No Sequence)");
        return;
    }

    if (ImGui::SmallButton("Flip180##flip_yaw180"))
        FlipKeys_Yaw180();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(14.f, 0.f));
    ImGui::SameLine();

    bool changedAny = false;

    auto HandleEqual = [](OBJECT_HANDLE a, OBJECT_HANDLE b) -> bool
        {
            return a.hObjID == b.hObjID && a.Level == b.Level && a.Layer == b.Layer;
        };

    auto MakeHandleLabel = [](OBJECT_HANDLE h) -> string
        {
            if (!h.isValid()) return "(None)";

            auto obj = OBJ->Request_Object(h);
            if (!obj) return "(Missing)";

            const string& name = obj->Get_InstanceName();

            string s;
            s.reserve(name.size() + h.Layer.size() + 8);

            s += name;
            s += " (";
            s += h.Layer;
            s += ")";

            return s;
        };

    ImGui::PushID("InterpInline");

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Pos");
    ImGui::SameLine();
    {
        const CamPosInterp shown = target.sequence->posInterp;

        if (Helper::DrawEnumCombo("##pos_interp", target.sequence->posInterp, shown, 140.f))
        {
            if (target.sequence->posInterp == CamPosInterp::OrbitArc) target.sequence->orbitArc.enabled = true;
            changedAny = true;
        }
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Rot");
    ImGui::SameLine();
    {
        const CamRotInterp shown = target.sequence->rotInterp;
        if (Helper::DrawEnumCombo("##rot_interp", target.sequence->rotInterp, shown, 140.f)) changedAny = true;
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("FOV");
    ImGui::SameLine();
    {
        const CamFovInterp shown = target.sequence->fovInterp;
        if (Helper::DrawEnumCombo("##fov_interp", target.sequence->fovInterp, shown, 140.f)) changedAny = true;
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Ease");
    ImGui::SameLine();
    {
        const EaseType shown = target.sequence->segmentEase;

        ImGui::SetNextItemWidth(140.f);
        if (ImGui::BeginCombo("##seg_ease", Helper::EnumLabel(shown)))
        {
            if (DrawEaseComboPopup(target.sequence->segmentEase, shown)) changedAny = true;
            ImGui::EndCombo();
        }
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Space");
    ImGui::SameLine();
    {
        const CamSpace prevSpace = target.sequence->space;
        const CamSpace shown = target.sequence->space;

        if (Helper::DrawEnumCombo("##space", target.sequence->space, shown, 120.f))
        {
            if (target.sequence->space == CamSpace::Local && !target.spaceRefHandle.isValid() && !spaceRefCandidates.empty())
                target.spaceRefHandle = spaceRefCandidates[0];

            if (target.sequence->space == CamSpace::Local && !target.spaceRefHandle.isValid())
            {
                keyListUI.lastFileError = "Local Space needs Ref";
                keyListUI.requestOpenFileErrorPopup = true;
                target.sequence->space = prevSpace;
            }
            else
            {
                ConvertKeysSpace(target.sequence->keyframes, prevSpace, target.sequence->space, target.spaceRefHandle);

                if (target.player)
                {
                    if (target.sequence->space == CamSpace::Local) target.player->SetSpaceReference(target.spaceRefHandle);
                    else target.player->ClearSpaceReference();
                }

                changedAny = true;
            }
        }
    }

    if (target.sequence->space == CamSpace::Local)
    {
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(10.f, 0.f));
        ImGui::SameLine();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Ref");
        ImGui::SameLine();

        const string previewRef = MakeHandleLabel(target.spaceRefHandle);

        ImGui::SetNextItemWidth(260.f);
        if (ImGui::BeginCombo("##space_ref", previewRef.c_str()))
        {
            for (OBJECT_HANDLE h : spaceRefCandidates)
            {
                const string item = MakeHandleLabel(h);
                const bool selected = HandleEqual(h, target.spaceRefHandle);

                if (ImGui::Selectable(item.c_str(), selected))
                {
                    OBJECT_HANDLE oldRef = target.spaceRefHandle;
                    OBJECT_HANDLE newRef = h;

                    if (oldRef.isValid())
                    {
                        ConvertKeysSpace(target.sequence->keyframes, CamSpace::Local, CamSpace::World, oldRef);
                        ConvertKeysSpace(target.sequence->keyframes, CamSpace::World, CamSpace::Local, newRef);
                    }

                    target.spaceRefHandle = newRef;

                    if (target.player) target.player->SetSpaceReference(target.spaceRefHandle);

                    changedAny = true;
                }

                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        animGUIController.DrawInline(target.spaceRefHandle);
        DrawPlayAll(target.spaceRefHandle);
    }

    ImGui::PopID();

    if (changedAny) PostEdit_SequenceChanged();
}


void CCamPanel::DrawKeyframeList()
{
    if (!target.sequence)
    {
        ImGui::TextDisabled("No sequence selected.");
        return;
    }

    EnsureNameBufSync();

    auto& keys = GetKeyFrames();
    bool changedAny = false;

    DrawKeyframeList_TopBar(keys, changedAny);
    DrawKeyframeList_HeaderArea(keys, changedAny);
    DrawKeyframeList_Table(keys, changedAny);

    if (changedAny)
        PostEdit_SequenceChanged();
}

void CCamPanel::DrawKeyframeEditor()
{
    ImGui::SeparatorText("Selected Key");

    if (!HasValidSelection())
    {
        ImGui::TextDisabled("Select a keyframe from the list.");
        return;
    }

    const bool showOrbitArc = target.sequence && (target.sequence->posInterp == CamPosInterp::OrbitArc);

    ImGuiTableFlags splitFlags =
        ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_PadOuterX;

    int cols = showOrbitArc ? 2 : 1;

    if (ImGui::BeginTable("SelectedKeySplit", cols, splitFlags, ImVec2(0.f, 0.f)))
    {
        if (showOrbitArc)
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 420.f);
            ImGui::TableSetupColumn("OrbitArc", ImGuiTableColumnFlags_WidthStretch);
        }
        else
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        bool changedAny = false;
        bool changedOrbit = false;

        ImGui::TableSetColumnIndex(0);
        DrawKeyframeEditor_SelectedKeyTable(changedAny);

        if (showOrbitArc)
        {
            ImGui::TableSetColumnIndex(1);
            DrawKeyframeEditor_OrbitArc(changedOrbit);
        }

        if (changedAny || changedOrbit)
            PostEdit_SequenceChanged();

        ImGui::EndTable();
    }
}

void CCamPanel::DrawTimeline()
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

            float k01 = clamp(k.time / endT, 0.f, 1.f);
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
        ImGui::Text("t = %.2fs", hoverTime);

        const float snapPx = 7.f;
        if (nearest && bestDx <= snapPx)
        {
            ImGui::Separator();
            ImGui::Text("Key #%03u", nearest->keyId);
            ImGui::Text("Time : %.2fs", nearest->time);
            ImGui::Text("Pos  : %.2f, %.2f, %.2f", nearest->pos.x, nearest->pos.y, nearest->pos.z);
            ImGui::Text("Look : %.2f, %.2f, %.2f", nearest->look.x, nearest->look.y, nearest->look.z);
            ImGui::Text("Roll : %.2f", nearest->roll);
            ImGui::Text("FOV  : %.2f", nearest->fov);
        }
        ImGui::EndTooltip();
    }

    if (ImGui::IsItemActive())
    {
        float mx = ImGui::GetIO().MousePos.x;
        float local = clamp((mx - barPos.x) / barSize.x, 0.f, 1.f);
        state.curTime = local * endT;

        if (target.player && !state.recording)
            target.player->SetTime(state.curTime);
    }
}

void CCamPanel::DrawHelpPopup()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::BeginPopupModal("CameraTool_Help", nullptr, flags))
        return;

    ImGui::SeparatorText(u8"Camera Tool Guide");

    if (ImGui::BeginTabBar("##help_tabs"))
    {
        if (ImGui::BeginTabItem(u8"Quick Start"))
        {
            ImGui::TextDisabled(u8"가장 빠른 사용 흐름");
            ImGui::Spacing();

            ImGui::BulletText(u8"1) CAPTURE ON");
            ImGui::Indent();
            ImGui::TextUnformatted(u8"- DebugFreeCam으로 원하는 구도/위치로 이동");
            ImGui::TextUnformatted(u8"- Key Editor의 Capture 버튼으로 선택 키프레임에 기록");
            ImGui::Unindent();

            ImGui::BulletText(u8"2) CAPTURE OFF");
            ImGui::Indent();
            ImGui::TextUnformatted(u8"- Play로 재생하며 보간/연출 확인");
            ImGui::TextUnformatted(u8"- 타임라인 드래그로 구간별 문제를 빠르게 찾기");
            ImGui::Unindent();

            ImGui::BulletText(u8"3) 키 추가/이동");
            ImGui::Indent();
            ImGui::TextUnformatted(u8"- + Add : 키 추가 (기본은 마지막 키 복사)");
            ImGui::TextUnformatted(u8"- Go(>) : 그 키 시간으로 커서 이동");
            ImGui::TextUnformatted(u8"- 리스트 더블클릭 : 그 키로 점프");
            ImGui::Unindent();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled(u8"주의");
            ImGui::BulletText(u8"키가 1개 이하이면 재생/보간이 '고정'처럼 보일 수 있어요.");
            ImGui::BulletText(u8"Time을 바꿨는데 키가 사라진 것 같으면, 같은 시간대 덮어쓰기(merge) 가능성을 확인하세요.");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"Keyframe"))
        {
            ImGui::TextDisabled(u8"키프레임 단위로 바뀌는 것들");
            ImGui::Spacing();

            ImGui::BulletText(u8"Custom Interp");
            ImGui::Indent();
            ImGui::TextUnformatted(u8"- 그 키에서 다음 키까지 구간의 Pos/Rot/FOV 보간을 개별 지정");
            ImGui::TextUnformatted(u8"- 체크 시 기본값(Sequence 설정)을 복사해 시작하므로 안전");
            ImGui::Unindent();

            ImGui::BulletText(u8"Custom Ease");
            ImGui::Indent();
            ImGui::TextUnformatted(u8"- 그 키(출발 키)에서 다음 키까지 구간의 Easing만 개별 지정");
            ImGui::TextUnformatted(u8"- 즉, outEase는 '출발 키의 구간 설정'이에요");
            ImGui::Unindent();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled(u8"Time 변경");
            ImGui::BulletText(u8"같은 시간대 키가 있으면 덮어쓰기 경고가 뜹니다.");
            ImGui::BulletText(u8"덮어쓰면 '가까운 시간 키들'이 merge 정책에 의해 정리될 수 있어요.");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"Easing"))
        {
            static EaseType selected = EaseType::InOutSine;

            ImGui::TextDisabled(u8"개념");
            ImGui::BulletText(u8"Ease는 시간 u(0~1)을 다시 매핑해서 같은 구간에서도 체감 속도를 바꿔요.");
            ImGui::BulletText(u8"Sequence.segmentEase : 전체 구간 기본 Ease");
            ImGui::BulletText(u8"Key.useCustomEase + Key.outEase : 특정 구간만 Override");

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(u8"Preview");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(220.f);
            if (ImGui::BeginCombo("##ease_preview_combo", Helper::EnumLabel(selected)))
            {
                DrawEaseComboPopup(selected, selected);
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            ImGui::TextDisabled(u8"(대각선 = Linear 기준)");

            ImGui::Spacing();

            ImVec2 graphSize(520.f, 220.f);
            DrawEaseGraph(selected, graphSize, "##ease_graph_preview");

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::TextDisabled(u8"빠른 선택 규칙");
            ImGui::BulletText(u8"일반 이동/팔로우: InOutSine");
            ImGui::BulletText(u8"빠르게 접근 후 자연스런 감속: OutCubic / OutQuint");
            ImGui::BulletText(u8"컷 진입을 숨기고 싶다: InCubic / InQuart");
            ImGui::BulletText(u8"착 붙는 정착감: OutCirc / InOutCirc");
            ImGui::BulletText(u8"그래프에 Overshoot가 뜨면(Back/Elastic/Bounce 계열) 카메라에서는 멀미 주의");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"OrbitArc"))
        {
            ImGui::TextDisabled(u8"OrbitArc는 직선 대신 원호로 이동하는 모드");
            ImGui::BulletText(u8"Center: 원의 중심");
            ImGui::BulletText(u8"Axis: 회전 축(정규화 필요)");
            ImGui::BulletText(u8"AngleMode: Shortest/Longest/Force180");
            ImGui::BulletText(u8"CW: 회전 방향 강제");
            ImGui::BulletText(u8"RadiusMode: Start/End/Blend");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled(u8"Auto");
            ImGui::BulletText(u8"Auto 180: 두 키 사이를 기준으로 중심/축을 잡고 180도 원호로 구성");
            ImGui::BulletText(u8"Auto Center/Axis: 중심 또는 축만 빠르게 세팅");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"Shortcuts"))
        {
            ImGui::TextDisabled(u8"자주 쓰는 조작");
            ImGui::BulletText(u8"타임라인 드래그: 커서 이동");
            ImGui::BulletText(u8"타임라인 Hover: 시간/가까운 키 정보 Tooltip");
            ImGui::BulletText(u8"키 리스트 더블클릭: 해당 키 시간으로 이동");
            ImGui::BulletText(u8"Go(>): 해당 키로 점프");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"Troubleshooting"))
        {
            ImGui::TextDisabled(u8"자주 나오는 문제");
            ImGui::BulletText(u8"Play를 눌렀는데 안 움직임: CAPTURE가 켜져 있으면 Player Apply가 꺼져 있을 수 있어요.");
            ImGui::BulletText(u8"키가 갑자기 사라짐: 같은 시간대 merge(덮어쓰기)로 정리됐을 수 있어요.");
            ImGui::BulletText(u8"Easing이 선택돼도 표시가 안 바뀜: Ease 라벨/콤보 목록이 30개를 다 지원하는지 확인하세요.");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Separator();

    if (ImGui::Button("Close", ImVec2(120.f, 0.f)))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

void CCamPanel::DrawWindowHeader()
{
    float y = ImGui::GetCursorPosY();
    if (y > 2.f) ImGui::SetCursorPosY(y - 2.f);

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Camera Tool");
    ImGui::SameLine(0.f, 10.f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.f, 4.f));
    bool open = ImGui::SmallButton("Help");
    ImGui::PopStyleVar();

    ImGui::SameLine(0.f, 6.f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.f, 4.f));
    if (ImGui::SmallButton("Hide"))
        panelUI.hidden = true;
    ImGui::PopStyleVar();

    if (open) ImGui::OpenPopup("CameraTool_Help");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"카메라툴 사용법 / Easing 가이드 / 단축키");

    DrawHelpPopup();
    ImGui::Separator();
}

void CCamPanel::DrawHiddenHandle()
{
    const ImVec2 display = ImGui::GetIO().DisplaySize;

    ImVec2 pos(200.f, display.y);
    pos.x = floorf(pos.x);
    pos.y = floorf(pos.y);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.f, 1.f));
    ImGui::SetNextWindowBgAlpha(0.9f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(170.f, 0.f), ImGuiCond_Always);

    if (ImGui::Begin("##CamToolHiddenHandle", nullptr, flags))
    {
        if (ImGui::Button("Camera Tool  ^", ImVec2(0.f, 0.f)))
            panelUI.hidden = false;
    }
    ImGui::End();
}

_bool CCamPanel::DrawConstraintBar()
{
    bool changed = false;
    const char* fmt = "%.2f";

    ImGui::PushID("MoveConstraintBar");

    const ImGuiStyle& style = ImGui::GetStyle();
    const float gapS = 4.f;
    const float gapM = 6.f;

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("Constraint");
    ImGui::SameLine(0.f, gapS);

    if (Helper::DrawEnumCombo("##constraint", state.moveConstraint, 95.f)) changed = true;

    if (state.moveConstraint == CamMoveConstraint::Orbit)
    {
        ImGui::SameLine(0.f, gapM);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, style.ItemSpacing.y));

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("LookAt");
        ImGui::SameLine(0.f, gapS);
        if (ImGui::Checkbox("##lookat", &state.orbit.lookAtCenter)) changed = true;

        ImGui::SameLine(0.f, gapM);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Dist");
        ImGui::SameLine(0.f, gapS);
        ImGui::SetNextItemWidth(58.f);
        if (ImGui::DragFloat("##dist", &state.orbit.distance, 0.1f, 0.1f, 9999.f, fmt)) changed = true;

        ImGui::SameLine(0.f, gapM);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Deg/s");
        ImGui::SameLine(0.f, gapS);
        ImGui::SetNextItemWidth(70.f);
        if (ImGui::DragFloat("##degps", &state.orbit.angularSpeedDeg, 1.f, 0.f, 9999.f, fmt)) changed = true;

        ImGui::SameLine(0.f, gapM);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Center");
        ImGui::SameLine(0.f, gapS);
        if (ImGui::Checkbox("##use_center", &state.orbit.useCustomCenter)) changed = true;

        if (state.orbit.useCustomCenter)
        {
            ImGui::SameLine(0.f, gapM);

            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("X");
            ImGui::SameLine(0.f, gapS);
            ImGui::SetNextItemWidth(58.f);
            if (ImGui::DragFloat("##cx", &state.orbit.center.x, 0.1f, -99999.f, 99999.f, fmt)) changed = true;

            ImGui::SameLine(0.f, gapM);

            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("Y");
            ImGui::SameLine(0.f, gapS);
            ImGui::SetNextItemWidth(58.f);
            if (ImGui::DragFloat("##cy", &state.orbit.center.y, 0.1f, -99999.f, 99999.f, fmt)) changed = true;

            ImGui::SameLine(0.f, gapM);

            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("Z");
            ImGui::SameLine(0.f, gapS);
            ImGui::SetNextItemWidth(58.f);
            if (ImGui::DragFloat("##cz", &state.orbit.center.z, 0.1f, -99999.f, 99999.f, fmt)) changed = true;
        }

        ImGui::PopStyleVar();
    }

    ImGui::PopID();
    return changed;
}

_bool CCamPanel::DrawOrbitTargetBar()
{
    if (state.moveConstraint != CamMoveConstraint::Orbit) return false;

    bool changed = false;
    const char* fmt = "%.2f";

    ImGui::PushID("OrbitTargetBar");

    const ImGuiStyle& style = ImGui::GetStyle();
    const float gapS = 4.f;
    const float gapM = 6.f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, style.ItemSpacing.y));

    if (!state.orbit.lookAtCenter)
        ImGui::BeginDisabled();

    DrawLabelDisabled("Target");
    ImGui::SameLine(0.f, gapS);

    _vector3 targetPos = state.orbit.targetPos;
    if (DragVec3XYZ("target", targetPos, 0.1f, -99999.f, 99999.f, fmt, 58.f, gapM))
    {
        state.orbit.targetPos = targetPos;
        changed = true;
    }

    ImGui::SameLine(0.f, gapM);

    DrawLabelDisabled("OffY");
    ImGui::SameLine(0.f, gapS);

    if (DragFloat("##offy", state.orbit.offsetY, 0.1f, -99999.f, 99999.f, fmt, 58.f))
        changed = true;

    if (!state.orbit.lookAtCenter)
        ImGui::EndDisabled();

    ImGui::PopStyleVar();
    ImGui::PopID();

    return changed;
}

void CCamPanel::DrawPath()
{
    if (!target.sequence) return;

    const auto& keys = target.sequence->keyframes;
    if (keys.size() < 2) return;

    const Matrix view = *CameraManager()->Get_ViewMatrix();
    const Matrix proj = *CameraManager()->Get_ProjMatrix();

    Vector2 vp = GameInstance()->Get_ClientSize();
    const Vector4 viewport(0.f, 0.f, vp.x, vp.y);

    Matrix refRT = Matrix::Identity;
    if (target.sequence->space == CamSpace::Local && target.spaceRefHandle.isValid())
        refRT = GetRefRT(target.spaceRefHandle);

    Matrix invView = view.Invert();
    Vector3 camPos(invView._41, invView._42, invView._43);

    const float depthNear = 1.0f;
    const float depthFar = 25.0f;

    const int samplesPerSeg = 48;

    auto ToWorld = [&](const CamKeyFrame& k) -> Vector3
        {
            _vector3 wp = k.pos;
            if (target.sequence->space == CamSpace::Local) wp = ToWorldPos(k.pos, refRT);
            return Vector3(wp.x, wp.y, wp.z);
        };

    auto DepthU = [&](const Vector3& p) -> float
        {
            const float d = (p - camPos).Length();
            float u = (d - depthNear) / (depthFar - depthNear);
            return clamp(u, 0.f, 1.f);
        };

    auto CatmullRom = [](const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) -> Vector3
        {
            const float t2 = t * t;
            const float t3 = t2 * t;

            return 0.5f * ((2.f * p1) +
                (-p0 + p2) * t +
                (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 +
                (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3);
        };

    vector<Vector3> rawWorld;
    rawWorld.reserve(keys.size());
    for (const auto& k : keys)
        rawWorld.push_back(ToWorld(k));

    vector<Vector3> curve;
    curve.reserve((rawWorld.size() - 1) * (samplesPerSeg + 1));

    const int n = (int)rawWorld.size();
    for (int i = 0; i < n - 1; ++i)
    {
        const Vector3 p1 = rawWorld[i];
        const Vector3 p2 = rawWorld[i + 1];

        const Vector3 p0 = (i - 1 >= 0) ? rawWorld[i - 1] : (p1 + (p1 - p2));
        const Vector3 p3 = (i + 2 < n) ? rawWorld[i + 2] : (p2 + (p2 - p1));

        int s0 = (i == 0) ? 0 : 1;
        for (int s = s0; s <= samplesPerSeg; ++s)
        {
            float t = (float)s / (float)samplesPerSeg;
            curve.push_back(CatmullRom(p0, p1, p2, p3, t));
        }
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    bool hasPrev = false;
    ImVec2 prevScreen{};
    float  prevU = 0.f;

    for (const auto& wp : curve)
    {
        Vector2 screen{};
        if (!Helper::WorldToScreen(_float3(wp.x, wp.y, wp.z), (_float2&)screen, view, proj, viewport))
        {
            hasPrev = false;
            continue;
        }

        const float u = DepthU(wp);

        if (hasPrev)
        {
            const float a = Math::Lerp(255.f, 60.f, (prevU + u) * 0.5f);
            const float t = Math::Lerp(3.0f, 1.2f, (prevU + u) * 0.5f);

            dl->AddLine(prevScreen, ImVec2(screen.x, screen.y), IM_COL32(0, 255, 255, (int)a), t);
        }

        prevScreen = ImVec2(screen.x, screen.y);
        prevU = u;
        hasPrev = true;
    }

    const bool hasSel = HasValidSelection();
    const _uint selId = hasSel ? GetSelectedKeyId() : 0;

    for (const auto& k : keys)
    {
        Vector3 wp = ToWorld(k);

        Vector2 screen{};
        if (!Helper::WorldToScreen(_float3(wp.x, wp.y, wp.z), (_float2&)screen, view, proj, viewport))
            continue;

        const float u = DepthU(wp);
        const bool isSel = hasSel && (k.keyId == selId);

        float r = isSel ? 6.f : 4.f;
        r *= Math::Lerp(1.15f, 0.65f, u);

        const int a = (int)Math::Lerp(255.f, 90.f, u);

        dl->AddCircleFilled(ImVec2(screen.x, screen.y), r,
            isSel ? IM_COL32(255, 255, 0, a) : IM_COL32(255, 255, 255, a), 12);
    }
}

void CCamPanel::DrawPlayAll(OBJECT_HANDLE spaceRefHandle)
{
    const bool hasCam = target.player != nullptr;
    const bool hasAnim = animGUIController.HasAnimator(spaceRefHandle);

    ImGui::SameLine(0.f, 6.f);

    if (!hasCam || !hasAnim) ImGui::BeginDisabled();

    auto HandleEqual = [](OBJECT_HANDLE a, OBJECT_HANDLE b) -> bool
        {
            return a.hObjID == b.hObjID && a.Level == b.Level && a.Layer == b.Layer;
        };

    const bool linkedThis = state.playAllLink && HandleEqual(state.playAllRefHandle, spaceRefHandle);
    const bool camPlaying = !state.recording && state.playing;
    const bool allPlaying = linkedThis && camPlaying;

    if (allPlaying)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.15f, 0.15f, 1.f));

    ImGui::PushID(target.player);

    if (ImGui::SmallButton(allPlaying ? "AllStop" : "AllPlay"))
    {
        if (allPlaying)
        {
            SetPlaying(false);

            state.playAllLink = false;
            state.playAllRefHandle = {};

            animGUIController.SetPlaying(spaceRefHandle, false);
        }
        else
        {
            const float eps = 1e-4f;

            SetRecording(false);

            if (state.endTime > 1e-6f && state.curTime >= state.endTime - eps)
                state.curTime = 0.f;

            state.playAllLink = true;
            state.playAllRefHandle = spaceRefHandle;

            animGUIController.SetPlaying(spaceRefHandle, false);
            animGUIController.SetTimeSec(spaceRefHandle, state.curTime);

            SetPlaying(true);
        }
    }

    if (allPlaying)
        ImGui::PopStyleColor();

    ImGui::SameLine(0.f, 4.f);

    if (ImGui::SmallButton("Reset##all_reset"))
    {
        SetPlaying(false);

        state.playAllLink = false;
        state.playAllRefHandle = {};

        animGUIController.SetPlaying(spaceRefHandle, false);

        state.curTime = 0.f;

        if (target.player && !state.recording)
            target.player->SetTime(state.curTime);

        animGUIController.SetTimeSec(spaceRefHandle, 0.f);
    }

    ImGui::PopID();

    if (!hasCam || !hasAnim) ImGui::EndDisabled();
}



void CCamPanel::SetRecording(_bool on)
{
    if (state.recording == on) return;

    state.recording = on;
    if (state.recording) state.playing = false;

    if (target.captureCamObj)
        target.captureCamObj->SetControlEnabled(state.recording);

    if (target.player)
    {
        target.player->SetApplyEnabled(!state.recording);
        target.player->SetTimeScale(state.timeScale);

        if (!state.recording)
            target.player->SetTime(state.curTime);
    }
}

void CCamPanel::SetPlaying(_bool on)
{
    if (state.playing == on) return;

    state.playing = on;

    if (state.playing && state.recording)
        SetRecording(false);

    if (target.player && !state.recording)
        target.player->SetTime(state.curTime);
}

void CCamPanel::SetSpaceReference(OBJECT_HANDLE handle)
{
    target.spaceRefHandle = handle;

    if (target.player && target.sequence && target.sequence->space == CamSpace::Local)
        target.player->SetSpaceReference(target.spaceRefHandle);

    ApplyActorVisibility();

    if (state.playAllLink)
    {
        state.playAllRefHandle = handle;
        animGUIController.SetPlaying(handle, false);
        animGUIController.SetTimeSec(handle, state.curTime);
    }
}

void CCamPanel::SetSpaceRefCandidates(initializer_list<OBJECT_HANDLE> handles)
{
    spaceRefCandidates.assign(handles.begin(), handles.end());

    if (!target.spaceRefHandle.isValid() && !spaceRefCandidates.empty())
        target.spaceRefHandle = spaceRefCandidates[0];

    if (target.player && target.sequence)
    {
        if (target.sequence->space == CamSpace::Local) target.player->SetSpaceReference(target.spaceRefHandle);
        else target.player->ClearSpaceReference();
    }

    ApplyActorVisibility();
}

void CCamPanel::RecalcEndTimeFromKeys()
{
    if (!target.sequence)
    {
        state.endTime = 0.f;
        return;
    }

    const auto& keys = GetKeyFrames();

    float maxT = 0.f;
    for (size_t i = 0; i < keys.size(); ++i)
        maxT = max(maxT, keys[i].time);

    state.endTime = maxT;
}

void CCamPanel::ClampCurTime()
{
    if (state.curTime < 0.f) state.curTime = 0.f;
    if (state.curTime > state.endTime) state.curTime = state.endTime;
}

void CCamPanel::PostEdit_SequenceChanged()
{
    if (!target.player) return;

    target.player->Invalidate();

    if (!state.recording)
        target.player->SetTime(state.curTime);
}

bool CCamPanel::HasValidSelection() const
{
    if (!target.sequence) return false;
    if (state.selectedKeyIdx < 0) return false;
    if (state.selectedKeyIdx >= (_int)target.sequence->keyframes.size()) return false;
    return true;
}

CamKeyFrame& CCamPanel::GetSelectedKey()
{
    assert(HasValidSelection());
    return target.sequence->keyframes[(size_t)state.selectedKeyIdx];
}

void CCamPanel::FlipKeys_Yaw180()
{
    assert(target.sequence);

    for (auto& k : target.sequence->keyframes)
    {
        k.pos.x = -k.pos.x;
        k.pos.z = -k.pos.z;

        k.look.x = -k.look.x;
        k.look.z = -k.look.z;
        k.look.Normalize();
    }

    if (target.sequence->posInterp == CamPosInterp::OrbitArc)
    {
        auto& d = target.sequence->orbitArc;

        d.center.x = -d.center.x;
        d.center.z = -d.center.z;

        d.axis.x = -d.axis.x;
        d.axis.z = -d.axis.z;
        d.NormalizeAxis();
    }

    if (HasValidSelection())
        SyncEditorFromSelection();

    PostEdit_SequenceChanged();
}

_uint CCamPanel::GetSelectedKeyId() const
{
    assert(HasValidSelection());
    return target.sequence->keyframes[(size_t)state.selectedKeyIdx].keyId;
}

_float CCamPanel::GetNextDefaultTime() const
{
    if (!target.sequence || target.sequence->keyframes.empty())
        return 0.f;

    const _float lastTime = target.sequence->keyframes.back().time;
    return lastTime + policy.defaultStepTime;
}

void CCamPanel::AddKey_Default()
{
    assert(target.sequence);

    CamKeyFrame newKey{};
    newKey.keyId = target.nextKeyId++;
    newKey.time = GetNextDefaultTime();

    newKey.useCustomInterp = false;
    newKey.outPosInterp = target.sequence->posInterp;
    newKey.outRotInterp = target.sequence->rotInterp;
    newKey.outFovInterp = target.sequence->fovInterp;

    newKey.useCustomEase = false;
    newKey.outEase = target.sequence->segmentEase;

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
    MergeNearDuplicateTimes();
    SelectKeyById(newKey.keyId);
    SyncEditorFromSelection();

    PostEdit_SequenceChanged();
}

void CCamPanel::DeleteSelectedKey()
{
    if (!HasValidSelection()) return;

    auto& keys = GetKeyFrames();
    assert(!keys.empty());

    const _int deleteIdx = state.selectedKeyIdx;
    keys.erase(keys.begin() + deleteIdx);

    state.playing = false;

    if (keys.empty())
    {
        state.selectedKeyIdx = -1;
        state.curTime = 0.f;

        PostEdit_SequenceChanged();
        return;
    }

    state.selectedKeyIdx = clamp(deleteIdx, 0, (_int)keys.size() - 1);
    SyncEditorFromSelection();

    PostEdit_SequenceChanged();
}

void CCamPanel::SortKeysByTime_Stable()
{
    auto& keys = GetKeyFrames();
    stable_sort(keys.begin(), keys.end(), [](const CamKeyFrame& a, const CamKeyFrame& b) { return a.time < b.time; });
}

void CCamPanel::MergeNearDuplicateTimes(_uint preferKeyId)
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
        {
            if (preferKeyId != 0)
            {
                const bool lastIsPrefer = (last.keyId == preferKeyId);
                const bool curIsPrefer = (cur.keyId == preferKeyId);

                if (lastIsPrefer && !curIsPrefer)
                    continue;

                last = cur;
            }
            else
                last = cur;
        }
        else
            merged.push_back(cur);
    }
    keys.swap(merged);
}

bool CCamPanel::SelectKeyById(_uint keyId)
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

void CCamPanel::SyncEditorFromSelection()
{
    if (!HasValidSelection()) return;

    const CamKeyFrame& key = GetSelectedKey();

    state.editTime = key.time;
    state.editFov = key.fov;
    state.editRoll = key.roll;

    state.curTime = key.time;

    if (state.endTime < key.time)
        state.endTime = key.time;

    if (target.player && !state.recording)
        target.player->SetTime(state.curTime);
}

void CCamPanel::ApplyEditorToSelectedKey_TimeOnly()
{
    if (!HasValidSelection()) return;

    auto& keys = GetKeyFrames();

    CamKeyFrame& selectedKey = GetSelectedKey();
    const _uint selectedId = selectedKey.keyId;

    const float newTime = max(0.f, state.editTime);
    selectedKey.time = newTime;

    SortKeysByTime_Stable();

    MergeNearDuplicateTimes(selectedId);

    SelectKeyById(selectedId);

    if (HasValidSelection())
        SyncEditorFromSelection();
    else
        state.selectedKeyIdx = -1;

    PostEdit_SequenceChanged();
}

void CCamPanel::CaptureSelectedKey_FromCaptureCam()
{
    if (!HasValidSelection()) return;
    if (!target.captureCamObj) return;

    CamKeyFrame& key = GetSelectedKey();

    auto camTr = target.captureCamObj->Get_Component<CTransform>();

    _vector3 worldPos = camTr->Get_Pos();
    _vector4 look4 = camTr->Dir(STATE::LOOK);
    _vector3 worldLook = { look4.x, look4.y, look4.z };
    worldLook.Normalize();

    if (target.sequence->space == CamSpace::Local)
    {
        auto refObj = OBJ->Request_Object(target.spaceRefHandle);
        auto refTr = refObj->Get_Component<CTransform>();

        Matrix refWorld = Matrix(refTr->Get_WorldMatrix());

        Vector3 s{};
        Vector3 t{};
        Quaternion r = Quaternion::Identity;
        refWorld.Decompose(s, r, t);
        r.Normalize();

        Matrix refRT = Matrix::CreateFromQuaternion(r) * Matrix::CreateTranslation(t);
        Matrix invRefRT = refRT.Invert();

        Vector3 lp = Vector3::Transform(Vector3(worldPos.x, worldPos.y, worldPos.z), invRefRT);
        Vector3 ll = Vector3::TransformNormal(Vector3(worldLook.x, worldLook.y, worldLook.z), invRefRT);
        ll.Normalize();

        key.pos = _vector3(lp.x, lp.y, lp.z);
        key.look = _vector3(ll.x, ll.y, ll.z);
    }
    else
    {
        key.pos = worldPos;
        key.look = worldLook;
    }

    if (target.captureCamComp)
        key.fov = target.captureCamComp->Get_FOV();

    key.roll = 0.f;

    SyncEditorFromSelection();
    PostEdit_SequenceChanged();
}

bool CCamPanel::ValidateCamPath(const string& pickedPath, string& outError) const
{
    if (pickedPath.empty())
        return false;

    if (Helper::ContainsNonAscii(pickedPath))
    {
        outError = "File Path Must Be English";
        return false;
    }

    if (!Helper::IsPathInProjectFolder(filesystem::path(pickedPath).parent_path().string()))
    {
        outError = "Folders outside the project folder cannot be selected";
        return false;
    }

    if (filesystem::path(pickedPath).extension().string() != ".cam")
    {
        outError = "File extension must be .cam";
        return false;
    }

    return true;
}

string CCamPanel::GetDefaultCamFileName() const
{
    string base = target.sequence ? target.sequence->name : string{};
    if (base.empty()) base = "CameraSequence";

    if (Helper::ContainsNonAscii(base))
        base = "CameraSequence";

    for (char& c : base)
    {
        if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '\"' || c == '<' || c == '>' || c == '|')
            c = '_';
    }

    if (filesystem::path(base).extension().string() != ".cam")
        base += ".cam";

    return base;
}

void CCamPanel::SyncNameBufFromSeq()
{
    if (!target.sequence)
        return;

    const string& n = target.sequence->name;
    const string fallback = "DebugSequence";
    const string& src = n.empty() ? fallback : n;
    strncpy_s(keyListUI.prefabNameBuf, src.c_str(), _TRUNCATE);
}

void CCamPanel::EnsureNameBufSync()
{
    if (!target.sequence)
        return;

    if (keyListUI.lastSeqPtr != target.sequence)
    {
        keyListUI.lastSeqPtr = target.sequence;
        SyncNameBufFromSeq();
        return;
    }

    if (!keyListUI.nameEditing)
    {
        if (target.sequence->name != string(keyListUI.prefabNameBuf))
            SyncNameBufFromSeq();
    }
}

void CCamPanel::DoSaveSequence()
{
    keyListUI.lastFileError.clear();

    const string defaultName = GetDefaultCamFileName();
    string picked = Helper::SaveFileDialogByWinAPI(defaultName, "cam");
    if (picked.empty()) return;

    string err;
    if (!ValidateCamPath(picked, err))
    {
        keyListUI.lastFileError = err;
        keyListUI.requestOpenFileErrorPopup = true;
        return;
    }

    if (!CamUtil::Save(filesystem::path(picked), *target.sequence, &err))
    {
        keyListUI.lastFileError = err;
        keyListUI.requestOpenFileErrorPopup = true;
        return;
    }
}

void CCamPanel::DoLoadSequence()
{
    keyListUI.lastFileError.clear();

    string picked = Helper::OpenFile_Dialogue();
    if (picked.empty()) return;

    LoadSequenceFromPath(picked);
}

void CCamPanel::ApplyActorVisibility()
{
    if (!target.spaceRefHandle.isValid()) return;

    for (OBJECT_HANDLE h : spaceRefCandidates)
    {
        auto obj = OBJ->Request_Object(h);
        obj->Set_Alive(h == target.spaceRefHandle);
    }

    auto selectedObj = OBJ->Request_Object(target.spaceRefHandle);
    selectedObj->Set_Alive(true);
}

void CCamPanel::DrawKeyframeList_TopBar(vector<CamKeyFrame>& keys, bool& ioChangedAny)
{
    const ImVec2 btnSize(78.f, 0.f);

    if (ImGui::Button("+ Add", btnSize))
    {
        if (state.recording) AddKey_Default();
        else ImGui::OpenPopup("AddKey_Confirm_NotCapture");
    }

    const ConfirmResult addR = DrawConfirmPopupModal("AddKey_Confirm_NotCapture", nullptr,
        {u8"CAPTURE(REC) OFF 상태에서 키를 추가할까요?", u8"- 이 키는 카메라에서 캡쳐되지 않습니다.", u8"- 마지막 키 복사/기본값으로 생성되며, 이후 Capture로 덮어쓸 수 있습니다."},
        u8"추가", u8"취소", 120.f);

    if (addR == ConfirmResult::Ok) AddKey_Default();

    ImGui::SameLine();

    const bool canDelete = HasValidSelection();
    if (!canDelete) ImGui::BeginDisabled();

    if (ImGui::Button("- Delete", btnSize))
    {
        if (!state.recording && keys.size() <= 2)
        {
            keyListUI.pendingDeleteKeyId = GetSelectedKeyId();
            ImGui::OpenPopup("DeleteKey_Confirm_TooFewKeys");
        }
        else
            DeleteSelectedKey();
    }

    if (!canDelete) ImGui::EndDisabled();

    const ConfirmResult delR = DrawConfirmPopupModal("DeleteKey_Confirm_TooFewKeys", nullptr,
        {u8"키프레임이 2개 이하입니다.", u8"삭제하면 1개 이하가 되어 일부 보간/재생이 비정상일 수 있어요.", u8"그래도 삭제할까요?"},
        u8"삭제", u8"취소", 120.f);

    if (delR == ConfirmResult::Ok)
    {
        if (SelectKeyById(keyListUI.pendingDeleteKeyId)) DeleteSelectedKey();
    }

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
        MergeNearDuplicateTimes();
        SelectKeyById(dup.keyId);
        SyncEditorFromSelection();

        if (target.player) target.player->Invalidate();
    }

    if (!canDuplicate) ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();
    ImGui::TextDisabled("|");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();
    ImGui::TextDisabled("FILE");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(6.f, 0.f));
    ImGui::SameLine();

    if (ImGui::Button("Save", btnSize))
    {
        keyListUI.lastFileError.clear();

        if (target.sequence->keyframes.empty()) ImGui::OpenPopup("CamSeq_Save_EmptyConfirm");
        else DoSaveSequence();
    }

    ImGui::SameLine();

    if (ImGui::Button("Load", btnSize)) DoLoadSequence();

    ImGui::SameLine();

    if (ImGui::Button("Auto", btnSize)) ImGui::OpenPopup("CamSeq_AutoLoad");

    DrawAutoLoadPopup();

    const ConfirmResult saveEmptyR = DrawConfirmPopupModal("CamSeq_Save_EmptyConfirm", nullptr,
        {u8"키프레임이 0개입니다.", u8"그래도 저장할까요?"}, u8"저장", u8"취소", 120.f);

    if (saveEmptyR == ConfirmResult::Ok) DoSaveSequence();

    if (keyListUI.requestOpenFileErrorPopup)
    {
        ImGui::OpenPopup("CamSeq_FileError");
        keyListUI.requestOpenFileErrorPopup = false;
    }

    if (DrawOkPopupModalText("CamSeq_FileError", u8"File Error", keyListUI.lastFileError, "OK", 120.f))
        keyListUI.lastFileError.clear();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();
    ImGui::TextDisabled("|");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.f, 0.f));
    ImGui::SameLine();
    ImGui::TextDisabled("Actor");
    ImGui::SameLine();

    auto MakeHandleLabel = [](OBJECT_HANDLE h) -> string
        {
            if (!h.isValid()) return "(None)";

            auto obj = OBJ->Request_Object(h);
            if (!obj) return "(Missing)";

            const string& name = obj->Get_InstanceName();

            string s;
            s.reserve(name.size() + h.Layer.size() + 8);

            s += name;
            s += " (";
            s += h.Layer;
            s += ")";

            return s;
        };

    const string preview = MakeHandleLabel(target.spaceRefHandle);

    ImGui::SetNextItemWidth(220.f);
    if (ImGui::BeginCombo("##actor_combo", preview.c_str()))
    {
        for (OBJECT_HANDLE h : spaceRefCandidates)
        {
            const string item = MakeHandleLabel(h);
            const bool selected = (h == target.spaceRefHandle);

            if (ImGui::Selectable(item.c_str(), selected))
                SetSpaceReference(h);

            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    char summaryBuf[128];
    sprintf_s(summaryBuf, "Keys: %d  |  End: %.2fs", (int)keys.size(), state.endTime);

    float rightX = ImGui::GetWindowContentRegionMax().x;
    float curX = ImGui::GetCursorPosX();
    float textW = ImGui::CalcTextSize(summaryBuf).x;
    if (rightX - textW > curX + 10.f) ImGui::SetCursorPosX(rightX - textW);

    ImGui::TextDisabled("%s", summaryBuf);
}


void CCamPanel::DrawKeyframeList_HeaderArea(vector<CamKeyFrame>& keys, bool& ioChangedAny)
{
    ImGui::Separator();
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Keyframes");

    float constraintStartX = 0.f;
    float constraintW = 0.f;

    const float inputW = 220.f;
    const float pad = 6.f;
    const char* label = "Name";
    const float labelW = ImGui::CalcTextSize(label).x;
    const float totalW = labelW + pad + inputW;

    ImGui::SameLine();
    float nameX = ImGui::GetWindowContentRegionMax().x - totalW;
    if (nameX < ImGui::GetCursorPosX()) nameX = ImGui::GetCursorPosX();

    constraintStartX = ImGui::GetCursorPosX();
    float constraintMaxX = nameX - 10.f;
    constraintW = constraintMaxX - constraintStartX;

    bool constraintChanged = false;

    if (constraintW > 150.f)
    {
        ImVec2 clipMin = ImGui::GetCursorScreenPos();
        ImVec2 clipMax = ImVec2(clipMin.x + constraintW, clipMin.y + ImGui::GetFrameHeight());

        ImGui::PushClipRect(clipMin, clipMax, true);
        constraintChanged = DrawConstraintBar();
        ImGui::PopClipRect();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(nameX);

    ImGui::TextDisabled("%s", label);
    ImGui::SameLine(0.f, pad);

    ImGui::SetNextItemWidth(inputW);
    const bool enter = ImGui::InputText("##prefab_name", keyListUI.prefabNameBuf, IM_ARRAYSIZE(keyListUI.prefabNameBuf), ImGuiInputTextFlags_EnterReturnsTrue);

    keyListUI.nameEditing = ImGui::IsItemActive();

    if (enter || ImGui::IsItemDeactivatedAfterEdit())
    {
        string nextName = keyListUI.prefabNameBuf;

        if (nextName.empty())
        {
            keyListUI.lastFileError = "Name cannot be empty";
            SyncNameBufFromSeq();
            keyListUI.requestOpenFileErrorPopup = true;
        }
        else if (ContainsNonAscii(nextName))
        {
            keyListUI.lastFileError = "Name must be English";
            SyncNameBufFromSeq();
            keyListUI.requestOpenFileErrorPopup = true;
        }
        else
            target.sequence->name = nextName;
    }

    if (constraintChanged && target.captureCamObj && state.recording)
    {
        target.captureCamObj->SetMoveConstraint(state.moveConstraint);

        if (state.moveConstraint == CamMoveConstraint::Orbit)
            target.captureCamObj->SetOrbitState(state.orbit);
    }

    if (state.moveConstraint == CamMoveConstraint::Orbit)
    {
        bool orbitChanged = false;

        ImGui::Spacing();
        ImGui::SetCursorPosX(constraintStartX);

        if (constraintW > 150.f)
        {
            ImVec2 clipMin = ImGui::GetCursorScreenPos();
            ImVec2 clipMax = ImVec2(clipMin.x + constraintW, clipMin.y + ImGui::GetFrameHeight());

            ImGui::PushClipRect(clipMin, clipMax, true);
            orbitChanged = DrawOrbitTargetBar();
            ImGui::PopClipRect();
        }
        else
            orbitChanged = DrawOrbitTargetBar();

        if (orbitChanged && target.captureCamObj && state.recording)
            target.captureCamObj->SetOrbitState(state.orbit);
    }

    ImGui::Separator();
}

void CCamPanel::DrawKeyframeList_Table(vector<CamKeyFrame>& keys, bool& ioChangedAny)
{
    ImVec2 tableSize = ImGui::GetContentRegionAvail();

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;

    const ImGuiStyle& baseStyle = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(baseStyle.FramePadding.x, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(baseStyle.ItemSpacing.x, 1.f));

    if (ImGui::BeginTable("KeyframeTable", 5, tableFlags, tableSize))
    {
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 64.f);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 78.f);
        ImGui::TableSetupColumn("Gap", ImGuiTableColumnFlags_WidthFixed, 66.f);
        ImGui::TableSetupColumn("Go", ImGuiTableColumnFlags_WidthFixed, 36.f);
        ImGui::TableSetupColumn("Interp", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();

        const float rowH = ImGui::GetFrameHeight() + ImGui::GetStyle().CellPadding.y * 2.f;

        auto CellHit = [&](const char* id, bool& rowHovered, bool& rowClicked) -> bool
            {
                return CamPanelUtil::TableCellHit(id, rowH, rowHovered, rowClicked);
            };

        ImGuiListClipper clipper;
        clipper.Begin((int)keys.size(), rowH);

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                CamKeyFrame& key = keys[(size_t)i];
                const bool isSelected = (state.selectedKeyIdx == i);

                ImGui::TableNextRow(ImGuiTableRowFlags_None, rowH);
                ImGui::PushID(i);

                bool rowHovered = false;
                bool rowClicked = false;

                ImGui::TableSetColumnIndex(0);
                CellHit("##hit_id", rowHovered, rowClicked);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("#%03u", key.keyId);

                ImGui::TableSetColumnIndex(1);
                CellHit("##hit_time", rowHovered, rowClicked);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%.2fs", key.time);

                ImGui::TableSetColumnIndex(2);
                CellHit("##hit_gap", rowHovered, rowClicked);
                ImGui::AlignTextToFramePadding();
                if ((size_t)i + 1 < keys.size()) ImGui::Text("%.2fs", keys[(size_t)i + 1].time - key.time);
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(3);
                bool goCellClicked = CellHit("##hit_go", rowHovered, rowClicked);
                bool goClicked = ImGui::SmallButton(">");
                if (goClicked) state.curTime = key.time;
                else if (goCellClicked) rowClicked = true;

                if (rowClicked) { state.selectedKeyIdx = i; SyncEditorFromSelection(); }
                if (rowHovered && ImGui::IsMouseDoubleClicked(0)) state.curTime = key.time;

                if (isSelected) ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderActive));
                else if (rowHovered) ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderHovered));

                ImGui::TableSetColumnIndex(4);
                {
                    bool custom = key.useCustomInterp;
                    if (ImGui::Checkbox("##custom", &custom))
                    {
                        key.useCustomInterp = custom;

                        if (key.useCustomInterp)
                        {
                            key.outPosInterp = target.sequence->posInterp;
                            key.outRotInterp = target.sequence->rotInterp;
                            key.outFovInterp = target.sequence->fovInterp;
                        }

                        ioChangedAny = true;
                    }

                    ImGui::SameLine();

                    const CamPosInterp shownPos = key.useCustomInterp ? key.outPosInterp : target.sequence->posInterp;
                    const CamRotInterp shownRot = key.useCustomInterp ? key.outRotInterp : target.sequence->rotInterp;
                    const CamFovInterp shownFov = key.useCustomInterp ? key.outFovInterp : target.sequence->fovInterp;

                    if (!key.useCustomInterp) ImGui::BeginDisabled();

                    if (Helper::DrawEnumCombo("##pos", key.outPosInterp, shownPos, 95.f)) ioChangedAny = true;
                    ImGui::SameLine();
                    if (Helper::DrawEnumCombo("##rot", key.outRotInterp, shownRot, 80.f)) ioChangedAny = true;
                    ImGui::SameLine();
                    if (Helper::DrawEnumCombo("##fov", key.outFovInterp, shownFov, 85.f)) ioChangedAny = true;

                    if (!key.useCustomInterp) ImGui::EndDisabled();

                    ImGui::SameLine();
                    ImGui::Dummy(ImVec2(8.f, 0.f));
                    ImGui::SameLine();

                    bool customEase = key.useCustomEase;
                    if (ImGui::Checkbox("##custom_ease", &customEase))
                    {
                        key.useCustomEase = customEase;
                        if (key.useCustomEase) key.outEase = target.sequence->segmentEase;
                        ioChangedAny = true;
                    }

                    ImGui::SameLine();

                    const EaseType shownEase = key.useCustomEase ? key.outEase : target.sequence->segmentEase;

                    if (!key.useCustomEase) ImGui::BeginDisabled();

                    ImGui::SetNextItemWidth(110.f);
                    if (ImGui::BeginCombo("##ease", Helper::EnumLabel(shownEase)))
                    {
                        if (DrawEaseComboPopup(key.outEase, shownEase)) ioChangedAny = true;
                        ImGui::EndCombo();
                    }

                    if (!key.useCustomEase) ImGui::EndDisabled();
                }

                ImGui::PopID();
            }
        }

        ImGui::TableNextRow(ImGuiTableRowFlags_None, 6.f);
        ImGui::TableSetColumnIndex(0);
        ImGui::Dummy(ImVec2(0.f, 0.f));

        ImGui::EndTable();
    }

    ImGui::PopStyleVar(3);
}

void CCamPanel::DrawKeyframeEditor_SelectedKeyTable(bool& ioChangedAny)
{
    if (!HasValidSelection()) return;

    CamKeyFrame* keyPtr = &GetSelectedKey();

    constexpr float labelWidth = 70.f;
    constexpr float floatWidth = 150.f;
    constexpr float vecValueWidth = 80.f;
    constexpr float axisGap = 10.f;

    const char* fmtScalar = "%.2f";
    const char* fmtVec = "%.2f";

    auto CountOverwriteAtTime = [&](float t, _uint exceptId) -> int
        {
            if (!target.sequence) return 0;

            const auto& keys = GetKeyFrames();

            int count = 0;
            for (size_t i = 0; i < keys.size(); ++i)
            {
                const CamKeyFrame& k = keys[i];
                if (k.keyId == exceptId) continue;
                if (fabsf(k.time - t) <= policy.mergeEpsilon) ++count;
            }
            return count;
        };

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
            if (minV < maxV) return ImGui::DragFloat(id, &v, speed, minV, maxV, fmtVec);
            return ImGui::DragFloat(id, &v, speed, 0.f, 0.f, fmtVec);
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
            bool changed = ImGui::DragFloat("##val", &v, speed, minV, maxV, fmtScalar);
            ImGui::PopID();
            return changed;
        };

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_PadOuterX |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_BordersInnerV;

    if (ImGui::BeginTable("KeyEditorTable", 2, tableFlags))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        BeginRow("Time");
        {
            ImGui::PushID("time");
            ImGui::SetNextItemWidth(floatWidth);

            ImGui::DragFloat("##time", &state.editTime, 0.01f, 0.f, 9999.f, fmtScalar);
            if (state.editTime < 0.f) state.editTime = 0.f;

            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                const _uint selectedId = GetSelectedKeyId();
                const float newTime = max(0.f, state.editTime);

                const int overwriteCount = CountOverwriteAtTime(newTime, selectedId);

                if (overwriteCount > 0)
                {
                    keyEditUI.pendingTimeSelectedId = selectedId;
                    keyEditUI.pendingTimeValue = newTime;
                    keyEditUI.pendingOverwriteCount = overwriteCount;
                    keyEditUI.requestOpenTimeCollisionPopup = true;
                }
                else
                {
                    ApplyEditorToSelectedKey_TimeOnly();
                    if (!HasValidSelection())
                    {
                        ImGui::PopID();
                        ImGui::EndTable();
                        return;
                    }
                    keyPtr = &GetSelectedKey();
                }
            }

            ImGui::SameLine();
            ImGui::TextDisabled("sec");

            ImGui::PopID();
        }

        BeginRow("Pos");
        {
            _vector3 pos = keyPtr->pos;
            if (DrawVec3Row_LeftLabel("pos", pos, 0.05f))
            {
                keyPtr->pos = pos;
                ioChangedAny = true;
            }
        }

        BeginRow("Dist");
        {
            const _vector3 p = keyPtr->pos;
            const float dist = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);

            ImGui::AlignTextToFramePadding();
            ImGui::Text("%.2f", dist);
            ImGui::SameLine();

            if (target.sequence && target.sequence->space == CamSpace::Local) ImGui::TextDisabled("(to Ref)");
            else ImGui::TextDisabled("(to Origin)");
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
                    ioChangedAny = true;
                }
            }
        }

        BeginRow("LookAt");
        {
            if (!keyEditUI.lookTargetInited)
            {
                keyEditUI.lookTargetInited = true;
                keyEditUI.lookTargetPos = _vector3(0.f, 0.f, 0.f);
            }

            _vector3 tgt = keyEditUI.lookTargetPos;
            if (DrawVec3Row_LeftLabel("lookat_target", tgt, 0.05f))
                keyEditUI.lookTargetPos = tgt;

            ImGui::SameLine(0.f, 12.f);

            if (ImGui::SmallButton("Look"))
            {
                _vector3 dir = keyEditUI.lookTargetPos - keyPtr->pos;
                if (dir.LengthSquared() > 1e-8f)
                {
                    dir.Normalize();
                    keyPtr->look = dir;
                    ioChangedAny = true;
                }
            }

            ImGui::SameLine();
            if (target.sequence && target.sequence->space == CamSpace::Local) ImGui::TextDisabled("Local");
            else ImGui::TextDisabled("World");
        }

        BeginRow("Roll");
        {
            float roll = keyPtr->roll;
            if (DrawFloatRow("roll", roll, 0.01f, -XM_PI, XM_PI))
            {
                keyPtr->roll = roll;
                ioChangedAny = true;
            }
        }

        BeginRow("FOV");
        {
            float fov = keyPtr->fov;
            if (DrawFloatRow("fov", fov, 0.1f, 1.f, 179.f))
            {
                keyPtr->fov = fov;
                ioChangedAny = true;
            }
        }

        BeginRow("Capture");
        {
            const bool hasCaptureTarget = (target.captureCamObj != nullptr);

            if (!hasCaptureTarget) ImGui::BeginDisabled();

            ImGui::PushID("capture_action");
            if (ImGui::Button("Capture", ImVec2(140.f, 0.f)))
            {
                CaptureSelectedKey_FromCaptureCam();
                ioChangedAny = true;
            }
            ImGui::PopID();

            if (!hasCaptureTarget) ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::TextDisabled(hasCaptureTarget ? "(from DebugFreeCam)" : "(no capture camera)");
        }

        ImGui::EndTable();
    }

    if (keyEditUI.requestOpenTimeCollisionPopup)
    {
        ImGui::OpenPopup("TimeCollisionConfirm");
        keyEditUI.requestOpenTimeCollisionPopup = false;
    }

    char msg[256];
    sprintf_s(msg, u8"적용하면 기존 키 %d개가 덮어씌워져 제거됩니다.\n그래도 적용할까요?", keyEditUI.pendingOverwriteCount);

    const ConfirmResult timeR = DrawConfirmPopupModal("TimeCollisionConfirm", nullptr,
        {u8"같은 시간대에 키가 이미 있습니다.", msg}, u8"적용", u8"취소", 120.f);

    if (timeR == ConfirmResult::Ok)
    {
        if (SelectKeyById(keyEditUI.pendingTimeSelectedId) && HasValidSelection())
        {
            state.editTime = keyEditUI.pendingTimeValue;
            ApplyEditorToSelectedKey_TimeOnly();
        }
    }
    else if (timeR == ConfirmResult::Cancel)
    {
        if (SelectKeyById(keyEditUI.pendingTimeSelectedId) && HasValidSelection())
            SyncEditorFromSelection();
    }
}

void CCamPanel::DrawKeyframeEditor_OrbitArc(bool& ioChangedOrbit)
{
    CamOrbitArcDesc& d = target.sequence->orbitArc;

    auto GetAutoSegment = [&](_vector3& outP0, _vector3& outP1) -> bool
        {
            const auto& keys = GetKeyFrames();
            if (keys.size() < 2) return false;

            int i = state.selectedKeyIdx;
            if (i < 0 || i >= (int)keys.size()) i = 0;

            if (i + 1 < (int)keys.size()) { outP0 = keys[(size_t)i].pos; outP1 = keys[(size_t)i + 1].pos; return true; }
            if (i - 1 >= 0) { outP0 = keys[(size_t)i - 1].pos; outP1 = keys[(size_t)i].pos; return true; }

            return false;
        };

    auto ComputeAxisPerpChord = [&](_vector3 chord) -> _vector3
        {
            if (chord.LengthSquared() <= 1e-10f) return _vector3(0.f, 1.f, 0.f);

            chord.Normalize();

            _vector3 ref(0.f, 1.f, 0.f);
            if (fabsf(chord.Dot(ref)) > 0.95f) ref = _vector3(0.f, 0.f, 1.f);

            _vector3 axis = chord.Cross(ref);
            if (axis.LengthSquared() <= 1e-10f) return _vector3(0.f, 1.f, 0.f);

            axis.Normalize();
            return axis;
        };

    ImGui::SeparatorText("OrbitArc");

    DrawLabelDisabled("Enabled");
    ImGui::SameLine();
    if (ImGui::Checkbox("##oa_enabled", &d.enabled)) ioChangedOrbit = true;

    ImGui::SameLine(0.f, 12.f);

    const bool canAuto = (GetKeyFrames().size() >= 2);
    if (!canAuto) ImGui::BeginDisabled();

    if (ImGui::SmallButton("Auto 180##oa_auto180"))
    {
        _vector3 p0{}, p1{};
        if (GetAutoSegment(p0, p1))
        {
            const _vector3 chord = p1 - p0;

            d.enabled = true;
            d.center = (p0 + p1) * 0.5f;
            d.axis = ComputeAxisPerpChord(chord);

            d.angleMode = CamOrbitArcAngleMode::Force180;
            d.radiusMode = CamOrbitArcRadiusMode::FixedStartRadius;

            ioChangedOrbit = true;
        }
    }

    ImGui::SameLine();
    if (ImGui::SmallButton("Auto Center##oa_autocenter"))
    {
        _vector3 p0{}, p1{};
        if (GetAutoSegment(p0, p1)) { d.enabled = true; d.center = (p0 + p1) * 0.5f; ioChangedOrbit = true; }
    }

    ImGui::SameLine();
    if (ImGui::SmallButton("Auto Axis##oa_autoaxis"))
    {
        _vector3 p0{}, p1{};
        if (GetAutoSegment(p0, p1))
        {
            d.enabled = true;
            d.axis = ComputeAxisPerpChord(p1 - p0);
            d.NormalizeAxis();
            ioChangedOrbit = true;
        }
    }

    if (!canAuto) ImGui::EndDisabled();

    ImGui::Spacing();

    const float vecW = 90.f;
    const float gap = 10.f;

    DrawLabelDisabled("Center");
    ImGui::SameLine();
    _vector3 center = d.center;
    if (DragVec3XYZ("oa_center", center, 0.05f, -99999.f, 99999.f, "%.1f", vecW, gap)) { d.center = center; ioChangedOrbit = true; }

    DrawLabelDisabled("Axis");
    ImGui::SameLine();
    _vector3 axis = d.axis;
    if (DragVec3XYZ("oa_axis", axis, 0.01f, -1.f, 1.f, "%.2f", vecW, gap)) { d.axis = axis; d.NormalizeAxis(); ioChangedOrbit = true; }

    DrawLabelDisabled("Angle");
    ImGui::SameLine();
    if (DrawEnumCombo("##oa_angle", d.angleMode, 160.f)) ioChangedOrbit = true;

    ImGui::SameLine(0.f, 12.f);
    DrawLabelDisabled("CW");
    ImGui::SameLine();
    if (ImGui::Checkbox("##oa_cw", &d.clockwise)) ioChangedOrbit = true;

    DrawLabelDisabled("Radius");
    ImGui::SameLine();
    if (DrawEnumCombo("##oa_radius", d.radiusMode, 160.f)) ioChangedOrbit = true;
}

void CCamPanel::DrawAutoLoadPopup()
{
    if (!ImGui::BeginPopup("CamSeq_AutoLoad"))
        return;

    ImGui::TextDisabled(u8"Auto Load Path");
    ImGui::Separator();

    ImGui::TextUnformatted(u8"Path");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(520.f);
    ImGui::InputText("##autoload_path", keyListUI.autoLoadPathBuf, IM_ARRAYSIZE(keyListUI.autoLoadPathBuf));

    ImGui::Spacing();

    if (ImGui::Button("Use Last Loaded", ImVec2(160.f, 0.f)))
    {
        if (!keyListUI.lastLoadedPath.empty())
            strncpy_s(keyListUI.autoLoadPathBuf, keyListUI.lastLoadedPath.c_str(), _TRUNCATE);
    }

    ImGui::SameLine();

    if (ImGui::Button("Save Auto", ImVec2(120.f, 0.f)))
        SaveToolSettings();

    ImGui::SameLine();

    if (ImGui::Button("Clear", ImVec2(100.f, 0.f)))
    {
        keyListUI.autoLoadPathBuf[0] = 0;
        SaveToolSettings();
    }

    ImGui::SameLine();

    if (ImGui::Button("Load Now", ImVec2(120.f, 0.f)))
    {
        if (LoadSequenceFromPath(string(keyListUI.autoLoadPathBuf)))
            SaveToolSettings();
    }

    ImGui::Separator();
    ImGui::TextDisabled(u8"- .cam 확장자 생략 가능");
    ImGui::TextDisabled(u8"- 비어있으면 다음 실행 때 자동 로드 안 함");

    ImGui::EndPopup();
}

void CCamPanel::LoadToolSettings()
{
    const filesystem::path p = GetCamToolSettingsPath();

    ifstream in(p);
    if (!in.is_open()) return;

    string line;
    getline(in, line);

    if (line.size() >= IM_ARRAYSIZE(keyListUI.autoLoadPathBuf))
        line.resize(IM_ARRAYSIZE(keyListUI.autoLoadPathBuf) - 1);

    strncpy_s(keyListUI.autoLoadPathBuf, line.c_str(), _TRUNCATE);
}

void CCamPanel::SaveToolSettings()
{
    const filesystem::path p = GetCamToolSettingsPath();
    filesystem::create_directories(p.parent_path());

    ofstream out(p, ios::trunc);
    if (!out.is_open()) return;

    out << keyListUI.autoLoadPathBuf;
}

void CCamPanel::AutoLoadSequenceIfSet()
{
    if (keyListUI.didAutoLoadOnce) return;

    keyListUI.didAutoLoadOnce = true;

    const string raw = keyListUI.autoLoadPathBuf;
    if (raw.empty()) return;

    LoadSequenceFromPath(raw);
}

bool CCamPanel::LoadSequenceFromPath(const string& anyPath)
{
    keyListUI.lastFileError.clear();

    const string picked = NormalizeCamPathForLoad(anyPath);
    if (picked.empty()) return false;

    string err;
    if (!ValidateCamPath(picked, err))
    {
        keyListUI.lastFileError = err;
        keyListUI.requestOpenFileErrorPopup = true;
        return false;
    }

    CamSequenceDesc loaded{};
    if (!CamUtil::Load(filesystem::path(picked), loaded, &err))
    {
        keyListUI.lastFileError = err;
        keyListUI.requestOpenFileErrorPopup = true;
        return false;
    }

    *target.sequence = move(loaded);

    if (target.sequence->space == CamSpace::Local && !target.spaceRefHandle.isValid() && !spaceRefCandidates.empty())
        target.spaceRefHandle = spaceRefCandidates[0];

    _uint maxId = 0;
    for (size_t i = 0; i < target.sequence->keyframes.size(); ++i)
        maxId = max(maxId, target.sequence->keyframes[i].keyId);
    target.nextKeyId = maxId + 1;

    state.selectedKeyIdx = target.sequence->keyframes.empty() ? -1 : 0;
    state.playing = false;

    SyncNameBufFromSeq();

    if (HasValidSelection())
        SyncEditorFromSelection();

    if (target.player)
    {
        if (target.sequence->space == CamSpace::Local) target.player->SetSpaceReference(target.spaceRefHandle);
        else target.player->ClearSpaceReference();

        target.player->Invalidate();
        if (!state.recording) target.player->SetTime(state.curTime);
    }

    keyListUI.lastLoadedPath = picked;
    return true;
}


CCamPanel* CCamPanel::Create(GUI_CONTEXT* context)
{
    auto inst = new CCamPanel(context);
    inst->Init();
    return inst;
}

void CCamPanel::Free()
{
    __super::Free();
}