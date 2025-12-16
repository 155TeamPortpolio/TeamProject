#include "pch.h"
#include "CamPanel.h"
#include "Helper_Func.h"
#include "Engine_Math.h"

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
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,   ImVec2(10.f, 7.f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,    ImVec2(8.f, 8.f));
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
    static bool DrawEaseComboPopup(EaseType& ioValue, EaseType shownValue)
    {
        bool changed = false;

        auto Pick = [&](EaseType v)
            {
                const bool selected = (shownValue == v);
                if (ImGui::Selectable(Math::GetEaseLabel(v), selected))
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
    static float EvalEase(EaseType v, float u)
    {
        if (u < 0.f) u = 0.f;
        if (u > 1.f) u = 1.f;

        switch (v)
        {
        case EaseType::None:         return u;
        case EaseType::InOutSine:    return Math::EaseInOutSine(u);
        case EaseType::OutCubic:     return Math::EaseOutCubic(u);
        case EaseType::InOutCubic:   return Math::EaseInOutCubic(u);
        case EaseType::OutSine:      return Math::EaseOutSine(u);
        case EaseType::InOutQuad:    return Math::EaseInOutQuad(u);

        case EaseType::InSine:       return Math::EaseInSine(u);
        case EaseType::InCubic:      return Math::EaseInCubic(u);
        case EaseType::InQuad:       return Math::EaseInQuad(u);
        case EaseType::InCirc:       return Math::EaseInCirc(u);

        case EaseType::InOutCirc:    return Math::EaseInOutCirc(u);
        case EaseType::OutCirc:      return Math::EaseOutCirc(u);
        case EaseType::OutQuad:      return Math::EaseOutQuad(u);

        case EaseType::InQuart:      return Math::EaseInQuart(u);
        case EaseType::InQuint:      return Math::EaseInQuint(u);
        case EaseType::InOutQuart:   return Math::EaseInOutQuart(u);
        case EaseType::OutQuart:     return Math::EaseOutQuart(u);
        case EaseType::InOutQuint:   return Math::EaseInOutQuint(u);
        case EaseType::OutQuint:     return Math::EaseOutQuint(u);

        case EaseType::InOutExpo:    return Math::EaseInOutExpo(u);
        case EaseType::OutExpo:      return Math::EaseOutExpo(u);
        case EaseType::InExpo:       return Math::EaseInExpo(u);

        case EaseType::OutBack:      return Math::EaseOutBack(u);
        case EaseType::InOutBack:    return Math::EaseInOutBack(u);
        case EaseType::InBack:       return Math::EaseInBack(u);

        case EaseType::OutElastic:   return Math::EaseOutElastic(u);
        case EaseType::InOutElastic: return Math::EaseInOutElastic(u);
        case EaseType::InElastic:    return Math::EaseInElastic(u);

        case EaseType::OutBounce:    return Math::EaseOutBounce(u);
        case EaseType::InOutBounce:  return Math::EaseInOutBounce(u);
        case EaseType::InBounce:     return Math::EaseInBounce(u);
        default:                        return u;
        }
    }

    static void DrawEaseGraph(EaseType ease, ImVec2 size)
    {
        if (size.x <= 10.f) size.x = 360.f;
        if (size.y <= 10.f) size.y = 180.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImVec2 p1(p0.x + size.x, p0.y + size.y);

        ImGui::InvisibleButton("##ease_graph", size);

        ImU32 colBg     = ImGui::GetColorU32(ImGuiCol_FrameBg);
        ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
        ImU32 colGrid   = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        ImU32 colLinear = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        ImU32 colCurve  = ImGui::GetColorU32(ImGuiCol_Text);
        ImU32 colWarn   = ImGui::GetColorU32(ImGuiCol_ButtonActive);

        dl->AddRectFilled(p0, p1, colBg, 6.f);
        dl->AddRect(p0, p1, colBorder, 6.f);

        const int samples = 160;
        float minY = 1e9f;
        float maxY = -1e9f;

        for (int i = 0; i <= samples; ++i)
        {
            float u = (float)i / (float)samples;
            float y = EvalEase(ease, u);
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }

        float lo = minY;
        float hi = maxY;

        if (lo > 0.f) lo = 0.f;
        if (hi < 1.f) hi = 1.f;

        float pad = (hi - lo) * 0.08f;
        if (pad < 0.02f) pad = 0.02f;

        lo -= pad;
        hi += pad;

        if (lo < -1.0f) lo = -1.0f;
        if (hi > 2.0f) hi = 2.0f;

        auto ToScreen = [&](float u, float y) -> ImVec2
            {
                float x01 = u;
                float y01 = (y - lo) / (hi - lo);
                if (y01 < 0.f) y01 = 0.f;
                if (y01 > 1.f) y01 = 1.f;

                float x = p0.x + x01 * size.x;
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

        bool overshoot = (minY < 0.f) || (maxY > 1.f);

        ImVec2 prev = ToScreen(0.f, EvalEase(ease, 0.f));
        for (int i = 1; i <= samples; ++i)
        {
            float u = (float)i / (float)samples;
            float y = EvalEase(ease, u);
            ImVec2 cur = ToScreen(u, y);
            dl->AddLine(prev, cur, overshoot ? colWarn : colCurve, 2.0f);
            prev = cur;
        }

        ImVec2 labelPos(p0.x + 10.f, p0.y + 8.f);
        dl->AddText(labelPos, ImGui::GetColorU32(ImGuiCol_Text), Math::GetEaseLabel(ease));

        char rangeBuf[128];
        sprintf_s(rangeBuf, "y range: %.2f .. %.2f", minY, maxY);
        dl->AddText(ImVec2(labelPos.x, labelPos.y + 18.f), ImGui::GetColorU32(ImGuiCol_TextDisabled), rangeBuf);

        if (overshoot)
            dl->AddText(ImVec2(labelPos.x, labelPos.y + 36.f), colWarn, "Overshoot");
    }
}

void CCamPanel::Init()
{
    debugSequence.name = "DebugSequence";
    debugSequence.camType = CamType::Cinematic;
    debugSequence.rigType = CamRigType::Free;
    debugSequence.projType = CamProjType::Perspective;
    debugSequence.playbackMode = CamPlaybackMode::Once;

    target.sequence = &debugSequence;
}

void CCamPanel::Update_Panel(_float dt)
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

    target.sequence->orbitArc.NormalizeAxis();

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
}

void CCamPanel::Render_GUI()
{
    constexpr float leftW = 200.f;
    constexpr float rightW = 250.f;
    constexpr float height = 400.f;

    const ImVec2 display = ImGui::GetIO().DisplaySize;

    ImVec2 bottomLeft(leftW, display.y);
    ImVec2 size(display.x - leftW - rightW, height);

    bottomLeft.x = floorf(bottomLeft.x);
    bottomLeft.y = floorf(bottomLeft.y);
    size.x = floorf(size.x);
    size.y = floorf(size.y);

    ImGui::SetNextWindowPos(bottomLeft, ImGuiCond_Always, ImVec2(0.f, 1.f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar;

    ScopedCamToolStyle styleScope;

    if (ImGui::Begin("Camera Tool##CamToolWindow", nullptr, flags))
    {
        DrawWindowHeader();

        DrawToolbar();
        ImGui::Separator();

        DrawCamSelector();
        ImGui::Separator();

        ImGuiTableFlags layoutFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV;

        if (ImGui::BeginTable("CamToolBodyLayout", 2, layoutFlags, ImVec2(0.f, 0.f)))
        {
            ImVec2 contentAvail = ImGui::GetContentRegionAvail();

            float minRight = 560.f;
            float minLeft = 360.f;

            float desiredLeft = contentAvail.x * 0.40f;
            float maxLeft = contentAvail.x - minRight;

            float leftColW = desiredLeft;
            leftColW = clamp(leftColW, minLeft, maxLeft);
            leftColW = max(leftColW, 300.f);

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

void CCamPanel::SetCaptureTarget(CCamObj* camObj)
{
    target.captureCamObj = camObj;
    target.captureCamComp = camObj ? camObj->Get_Component<CCamera>() : nullptr;
    target.player = camObj ? camObj->Get_Component<CCamSequencePlayer>() : nullptr;

    if (target.player && target.sequence)
        target.player->SetSequence(target.sequence);

    if (target.player)
        target.player->SetApplyEnabled(!state.recording);

    if (target.player && !state.recording)
        target.player->SetTime(state.curTime);

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
                state.recording = false;
        }
        else
            state.recording = nextRecording;

        if (target.captureCamObj)
            target.captureCamObj->SetControlEnabled(state.recording);
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

            if (target.captureCamObj)
                target.captureCamObj->SetControlEnabled(state.recording);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"계속 캡처", ImVec2(120.f, 0.f)))
        {
            state.recording = true;

            if (target.captureCamObj)
                target.captureCamObj->SetControlEnabled(state.recording);

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

        if (target.captureCamObj)
            target.captureCamObj->SetControlEnabled(state.recording);

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

void CCamPanel::DrawCamSelector()
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
        if (target.sequence->posInterp == CamPosInterp::CatmullRom)  posPreview = "CatmullRom";
        if (target.sequence->posInterp == CamPosInterp::Centripetal) posPreview = "Centripetal";
        if (target.sequence->posInterp == CamPosInterp::BSpline)     posPreview = "B-Spline";
        if (target.sequence->posInterp == CamPosInterp::OrbitArc)    posPreview = "OrbitArc";

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
            if (ImGui::Selectable("B-Spline", target.sequence->posInterp == CamPosInterp::BSpline))
            {
                target.sequence->posInterp = CamPosInterp::BSpline;
                changedAny = true;
            }
            if (ImGui::Selectable("OrbitArc", target.sequence->posInterp == CamPosInterp::OrbitArc))
            {
                target.sequence->posInterp = CamPosInterp::OrbitArc;
                target.sequence->orbitArc.enabled = true;
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
        const char* rotPreview = "Slerp";
        if (target.sequence->rotInterp == CamRotInterp::Squad) rotPreview = "Squad";
        if (target.sequence->rotInterp == CamRotInterp::Hold)  rotPreview = "Hold";

        ImGui::SetNextItemWidth(140.f);

        if (ImGui::BeginCombo("##rot_interp", rotPreview))
        {
            if (ImGui::Selectable("Slerp", target.sequence->rotInterp == CamRotInterp::Slerp))
            {
                target.sequence->rotInterp = CamRotInterp::Slerp;
                changedAny = true;
            }
            if (ImGui::Selectable("Squad", target.sequence->rotInterp == CamRotInterp::Squad))
            {
                target.sequence->rotInterp = CamRotInterp::Squad;
                changedAny = true;
            }
            if (ImGui::Selectable("Hold", target.sequence->rotInterp == CamRotInterp::Hold))
            {
                target.sequence->rotInterp = CamRotInterp::Hold;
                changedAny = true;
            }

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
        const char* fovPreview = "Linear";
        if (target.sequence->fovInterp == CamFovInterp::Smooth) fovPreview = "Smooth";
        if (target.sequence->fovInterp == CamFovInterp::Hold)   fovPreview = "Hold";

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
            if (ImGui::Selectable("Hold", target.sequence->fovInterp == CamFovInterp::Hold))
            {
                target.sequence->fovInterp = CamFovInterp::Hold;
                changedAny = true;
            }

            ImGui::EndCombo();
        }
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

        if (ImGui::BeginCombo("##seg_ease", Math::GetEaseLabel(shown)))
        {
            if (DrawEaseComboPopup(target.sequence->segmentEase, shown))
                changedAny = true;

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

void CCamPanel::DrawKeyframeArea()
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

void CCamPanel::DrawKeyframeList()
{
    if (!target.sequence)
    {
        ImGui::TextDisabled("No sequence selected.");
        return;
    }

    auto& keys = GetKeyFrames();
    const ImVec2 btnSize(78.f, 0.f);

    static _uint pendingDeleteKeyId = 0;
    static bool requestOpenDeletePopup = false;

    static string lastFileError{};
    static bool requestOpenEmptySaveConfirm = false;
    static bool requestOpenFileErrorPopup = false;

    static char prefabNameBuf[128] = "DebugSequence";
    static const CamSequenceDesc* lastSeqPtr = nullptr;
    static bool nameEditing = false;

    bool changedAny = false;

    auto ShowFileError = [&]()
        {
            if (!lastFileError.empty()) requestOpenFileErrorPopup = true;
        };

    auto SyncNameBufFromSeq = [&]()
        {
            const string& n = target.sequence->name;
            const string fallback = "DebugSequence";
            const string& src = n.empty() ? fallback : n;
            strncpy_s(prefabNameBuf, src.c_str(), _TRUNCATE);
        };

    if (lastSeqPtr != target.sequence)
    {
        lastSeqPtr = target.sequence;
        SyncNameBufFromSeq();
    }
    else
    {
        if (!nameEditing)
        {
            if (target.sequence->name != string(prefabNameBuf))
                SyncNameBufFromSeq();
        }
    }

    auto ValidateCamPath = [&](const string& pickedPath) -> bool
        {
            if (pickedPath.empty()) return false;

            if (Helper::ContainsNonAscii(pickedPath))
            {
                lastFileError = "File Path Must Be English";
                return false;
            }

            if (!Helper::IsPathInProjectFolder(filesystem::path(pickedPath).parent_path().string()))
            {
                lastFileError = "Folders outside the project folder cannot be selected";
                return false;
            }

            if (filesystem::path(pickedPath).extension().string() != ".cam")
            {
                lastFileError = "File extension must be .cam";
                return false;
            }

            return true;
        };

    auto GetDefaultCamFileName = [&]() -> string
        {
            string base = target.sequence->name;
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
        };

    auto DoSave = [&]()
        {
            lastFileError.clear();

            const string defaultName = GetDefaultCamFileName();
            string picked = Helper::SaveFileDialogByWinAPI(defaultName, "cam");
            if (picked.empty()) return;

            if (!ValidateCamPath(picked))
            {
                ShowFileError();
                return;
            }

            string err;
            if (!CamUtil::Save(filesystem::path(picked), *target.sequence, err))
            {
                lastFileError = err;
                ShowFileError();
                return;
            }
        };

    auto DoLoad = [&]()
        {
            lastFileError.clear();

            string picked = Helper::OpenFile_Dialogue();
            if (picked.empty()) return;

            if (filesystem::path(picked).extension().string() != ".cam")
            {
                lastFileError = "File extension must be .cam";
                ShowFileError();
                return;
            }

            CamSequenceDesc loaded{};
            string err;

            if (!CamUtil::Load(filesystem::path(picked), loaded, err))
            {
                lastFileError = err;
                ShowFileError();
                return;
            }

            *target.sequence = move(loaded);

            _uint maxId = 0;
            for (size_t i = 0; i < target.sequence->keyframes.size(); ++i)
                maxId = max(maxId, target.sequence->keyframes[i].keyId);
            target.nextKeyId = maxId + 1;

            state.selectedKeyIdx = target.sequence->keyframes.empty() ? -1 : 0;
            state.playing = false;

            SyncNameBufFromSeq();

            if (HasValidSelection()) SyncEditorFromSelection();

            if (target.player)
            {
                target.player->Invalidate();
                if (!state.recording) target.player->SetTime(state.curTime);
            }
        };

    if (ImGui::Button("+ Add", btnSize))
    {
        if (state.recording) AddKey_Default();
        else ImGui::OpenPopup("AddKey_Confirm_NotCapture");
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
    {
        if (!state.recording && keys.size() <= 2)
        {
            pendingDeleteKeyId = GetSelectedKeyId();
            requestOpenDeletePopup = true;
        }
        else
        {
            DeleteSelectedKey();
        }
    }

    if (!canDelete) ImGui::EndDisabled();

    if (requestOpenDeletePopup)
    {
        ImGui::OpenPopup("DeleteKey_Confirm_TooFewKeys");
        requestOpenDeletePopup = false;
    }

    if (ImGui::BeginPopupModal("DeleteKey_Confirm_TooFewKeys", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(u8"키프레임이 2개 이하입니다.");
        ImGui::Separator();
        ImGui::TextUnformatted(u8"삭제하면 1개 이하가 되어 일부 보간/재생이 비정상일 수 있어요.");
        ImGui::TextUnformatted(u8"그래도 삭제할까요?");
        ImGui::Separator();

        if (ImGui::Button(u8"삭제", ImVec2(120.f, 0.f)))
        {
            if (SelectKeyById(pendingDeleteKeyId)) DeleteSelectedKey();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"취소", ImVec2(120.f, 0.f)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
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
        MergeNearDuplicateTimes_KeepLast();
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
        lastFileError.clear();

        if (target.sequence->keyframes.empty()) requestOpenEmptySaveConfirm = true;
        else DoSave();
    }

    ImGui::SameLine();

    if (ImGui::Button("Load", btnSize))
        DoLoad();

    if (requestOpenEmptySaveConfirm)
    {
        ImGui::OpenPopup("CamSeq_Save_EmptyConfirm");
        requestOpenEmptySaveConfirm = false;
    }

    if (ImGui::BeginPopupModal("CamSeq_Save_EmptyConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(u8"키프레임이 0개입니다.");
        ImGui::Separator();
        ImGui::TextUnformatted(u8"그래도 저장할까요?");
        ImGui::Separator();

        if (ImGui::Button(u8"저장", ImVec2(120.f, 0.f)))
        {
            ImGui::CloseCurrentPopup();
            DoSave();
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"취소", ImVec2(120.f, 0.f)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    if (requestOpenFileErrorPopup)
    {
        ImGui::OpenPopup("CamSeq_FileError");
        requestOpenFileErrorPopup = false;
    }

    if (ImGui::BeginPopupModal("CamSeq_FileError", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(u8"File Error");
        ImGui::Separator();

        if (!lastFileError.empty())
            ImGui::TextUnformatted(lastFileError.c_str());

        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120.f, 0.f)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    char summaryBuf[128];
    sprintf_s(summaryBuf, "Keys: %d  |  End: %.1fs", (int)keys.size(), state.endTime);

    float rightX = ImGui::GetWindowContentRegionMax().x;
    float curX = ImGui::GetCursorPosX();
    float textW = ImGui::CalcTextSize(summaryBuf).x;
    if (rightX - textW > curX + 10.f) ImGui::SetCursorPosX(rightX - textW);

    ImGui::TextDisabled("%s", summaryBuf);

    ImGui::Separator();
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Keyframes");

    float constraintStartX = 0.f;
    float constraintW = 0.f;

    {
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
        const bool enter = ImGui::InputText("##prefab_name", prefabNameBuf, IM_ARRAYSIZE(prefabNameBuf), ImGuiInputTextFlags_EnterReturnsTrue);

        nameEditing = ImGui::IsItemActive();

        if (enter || ImGui::IsItemDeactivatedAfterEdit())
        {
            string nextName = prefabNameBuf;

            if (nextName.empty())
            {
                lastFileError = "Name cannot be empty";
                SyncNameBufFromSeq();
                ShowFileError();
            }
            else if (Helper::ContainsNonAscii(nextName))
            {
                lastFileError = "Name must be English";
                SyncNameBufFromSeq();
                ShowFileError();
            }
            else
            {
                target.sequence->name = nextName;
            }
        }

        if (constraintChanged && target.captureCamObj && state.recording)
        {
            target.captureCamObj->SetMoveConstraint(state.moveConstraint);

            if (state.moveConstraint == CamMoveConstraint::Orbit)
                target.captureCamObj->SetOrbitState(state.orbit);
        }
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
        {
            orbitChanged = DrawOrbitTargetBar();
        }

        if (orbitChanged && target.captureCamObj && state.recording)
            target.captureCamObj->SetOrbitState(state.orbit);
    }

    ImGui::Separator();

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

    auto GetPosLabel = [](_uint v) -> const char*
        {
            const CamPosInterp m = static_cast<CamPosInterp>(v);
            if (m == CamPosInterp::Linear)      return "Linear";
            if (m == CamPosInterp::CatmullRom)  return "Catmull";
            if (m == CamPosInterp::Centripetal) return "Centrip";
            if (m == CamPosInterp::BSpline)     return "B-Spline";
            if (m == CamPosInterp::Hermite)     return "Hermite";
            if (m == CamPosInterp::Hold)        return "Hold";
            if (m == CamPosInterp::OrbitArc)    return "OrbitArc";
            return "Unknown";
        };

    auto GetRotLabel = [](_uint v) -> const char*
        {
            const CamRotInterp m = static_cast<CamRotInterp>(v);
            if (m == CamRotInterp::Slerp) return "Slerp";
            if (m == CamRotInterp::Squad) return "Squad";
            if (m == CamRotInterp::Hold)  return "Hold";
            return "Unknown";
        };

    auto GetFovLabel = [](_uint v) -> const char*
        {
            const CamFovInterp m = static_cast<CamFovInterp>(v);
            if (m == CamFovInterp::Linear) return "Linear";
            if (m == CamFovInterp::Smooth) return "Smooth";
            if (m == CamFovInterp::Hold)   return "Hold";
            return "Unknown";
        };

    if (ImGui::BeginTable("KeyframeTable", 5, tableFlags, tableSize))
    {
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 64.f);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 78.f);
        ImGui::TableSetupColumn("Gap", ImGuiTableColumnFlags_WidthFixed, 66.f);
        ImGui::TableSetupColumn("Go", ImGuiTableColumnFlags_WidthFixed, 36.f);
        ImGui::TableSetupColumn("Interp", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();

        const ImGuiStyle& style = ImGui::GetStyle();
        const float frameH = ImGui::GetFrameHeight();
        const float rowH = frameH + style.CellPadding.y * 2.f;

        auto CellHit = [&](const char* id, bool& rowHovered, bool& rowClicked) -> bool
            {
                ImVec2 p = ImGui::GetCursorScreenPos();
                float w = ImGui::GetContentRegionAvail().x;

                ImGui::SetCursorScreenPos(ImVec2(p.x, p.y - style.CellPadding.y));
                ImGui::InvisibleButton(id, ImVec2(w, rowH));

                bool hovered = ImGui::IsItemHovered();
                bool clicked = ImGui::IsItemClicked(0);

                if (hovered) rowHovered = true;
                if (clicked) rowClicked = true;

                ImGui::SetItemAllowOverlap();
                ImGui::SetCursorScreenPos(p);

                return clicked;
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
                ImGui::Text("%.1fs", key.time);

                ImGui::TableSetColumnIndex(2);
                CellHit("##hit_gap", rowHovered, rowClicked);
                ImGui::AlignTextToFramePadding();
                if ((size_t)i + 1 < keys.size()) ImGui::Text("%.1fs", keys[(size_t)i + 1].time - key.time);
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(3);
                bool goCellClicked = CellHit("##hit_go", rowHovered, rowClicked);
                bool goClicked = ImGui::SmallButton(">");
                if (goClicked) state.curTime = key.time;
                else if (goCellClicked) rowClicked = true;

                if (rowClicked)
                {
                    state.selectedKeyIdx = i;
                    SyncEditorFromSelection();
                }

                if (rowHovered && ImGui::IsMouseDoubleClicked(0))
                    state.curTime = key.time;

                if (isSelected)
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderActive));
                else if (rowHovered)
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderHovered));

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

                        changedAny = true;
                    }

                    ImGui::SameLine();

                    const CamPosInterp shownPos = key.useCustomInterp ? key.outPosInterp : target.sequence->posInterp;
                    const CamRotInterp shownRot = key.useCustomInterp ? key.outRotInterp : target.sequence->rotInterp;
                    const CamFovInterp shownFov = key.useCustomInterp ? key.outFovInterp : target.sequence->fovInterp;

                    if (!key.useCustomInterp) ImGui::BeginDisabled();

                    ImGui::SetNextItemWidth(95.f);
                    if (ImGui::BeginCombo("##pos", GetPosLabel((_uint)shownPos)))
                    {
                        auto PickPos = [&](CamPosInterp v)
                            {
                                if (ImGui::Selectable(GetPosLabel((_uint)v), shownPos == v))
                                {
                                    key.outPosInterp = v;
                                    changedAny = true;
                                }
                            };

                        PickPos(CamPosInterp::Linear);
                        PickPos(CamPosInterp::CatmullRom);
                        PickPos(CamPosInterp::Centripetal);
                        PickPos(CamPosInterp::BSpline);
                        PickPos(CamPosInterp::Hermite);
                        PickPos(CamPosInterp::Hold);
                        PickPos(CamPosInterp::OrbitArc);

                        ImGui::EndCombo();
                    }

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(80.f);
                    if (ImGui::BeginCombo("##rot", GetRotLabel((_uint)shownRot)))
                    {
                        auto PickRot = [&](CamRotInterp v)
                            {
                                if (ImGui::Selectable(GetRotLabel((_uint)v), shownRot == v))
                                {
                                    key.outRotInterp = v;
                                    changedAny = true;
                                }
                            };

                        PickRot(CamRotInterp::Slerp);
                        PickRot(CamRotInterp::Squad);
                        PickRot(CamRotInterp::Hold);

                        ImGui::EndCombo();
                    }

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(85.f);
                    if (ImGui::BeginCombo("##fov", GetFovLabel((_uint)shownFov)))
                    {
                        auto PickFov = [&](CamFovInterp v)
                            {
                                if (ImGui::Selectable(GetFovLabel((_uint)v), shownFov == v))
                                {
                                    key.outFovInterp = v;
                                    changedAny = true;
                                }
                            };

                        PickFov(CamFovInterp::Linear);
                        PickFov(CamFovInterp::Smooth);
                        PickFov(CamFovInterp::Hold);

                        ImGui::EndCombo();
                    }

                    if (!key.useCustomInterp) ImGui::EndDisabled();

                    ImGui::SameLine();
                    ImGui::Dummy(ImVec2(8.f, 0.f));
                    ImGui::SameLine();

                    bool customEase = key.useCustomEase;
                    if (ImGui::Checkbox("##custom_ease", &customEase))
                    {
                        key.useCustomEase = customEase;
                        if (key.useCustomEase)
                            key.outEase = target.sequence->segmentEase;

                        changedAny = true;
                    }

                    ImGui::SameLine();

                    const EaseType shownEase = key.useCustomEase ? key.outEase : target.sequence->segmentEase;

                    if (!key.useCustomEase) ImGui::BeginDisabled();

                    ImGui::SetNextItemWidth(110.f);
                    if (ImGui::BeginCombo("##ease", Math::GetEaseLabel(shownEase)))
                    {
                        if (DrawEaseComboPopup(key.outEase, shownEase))
                            changedAny = true;

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

    if (changedAny && target.player)
    {
        target.player->Invalidate();
        if (!state.recording)
            target.player->SetTime(state.curTime);
    }
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
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
        }

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        {
            CamKeyFrame* keyPtr = &GetSelectedKey();

            constexpr float labelWidth = 70.f;
            constexpr float floatWidth = 150.f;
            constexpr float vecValueWidth = 80.f;
            constexpr float axisGap = 10.f;

            static _uint pendingTimeSelectedId = 0;
            static float pendingTimeValue = 0.f;
            static int pendingOverwriteCount = 0;
            static bool requestOpenTimeCollisionPopup = false;

            const char* fmtScalar = "%.1f";
            const char* fmtVec = "%.1f";

            bool changedAny = false;

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
                            pendingTimeSelectedId = selectedId;
                            pendingTimeValue = newTime;
                            pendingOverwriteCount = overwriteCount;
                            requestOpenTimeCollisionPopup = true;
                        }
                        else
                        {
                            ApplyEditorToSelectedKey_TimeOnly();
                            if (!HasValidSelection())
                            {
                                ImGui::PopID();
                                ImGui::EndTable();
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
                        changedAny = true;
                    }
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
                            changedAny = true;
                        }
                    }
                }

                BeginRow("Roll");
                {
                    float roll = keyPtr->roll;
                    if (DrawFloatRow("roll", roll, 0.01f, -XM_PI, XM_PI))
                    {
                        keyPtr->roll = roll;
                        changedAny = true;
                    }
                }

                BeginRow("FOV");
                {
                    float fov = keyPtr->fov;
                    if (DrawFloatRow("fov", fov, 0.1f, 1.f, 179.f))
                    {
                        keyPtr->fov = fov;
                        changedAny = true;
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
                        changedAny = true;
                    }
                    ImGui::PopID();

                    if (!hasCaptureTarget) ImGui::EndDisabled();

                    ImGui::SameLine();
                    ImGui::TextDisabled(hasCaptureTarget ? "(from DebugFreeCam)" : "(no capture camera)");
                }

                ImGui::EndTable();
            }

            if (requestOpenTimeCollisionPopup)
            {
                ImGui::OpenPopup("TimeCollisionConfirm");
                requestOpenTimeCollisionPopup = false;
            }

            if (ImGui::BeginPopupModal("TimeCollisionConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted(u8"같은 시간대에 키가 이미 있습니다.");
                ImGui::Separator();

                char msg[256];
                sprintf_s(msg, u8"적용하면 기존 키 %d개가 덮어씌워져 제거됩니다.\n그래도 적용할까요?", pendingOverwriteCount);
                ImGui::TextUnformatted(msg);

                ImGui::Separator();

                if (ImGui::Button(u8"적용", ImVec2(120.f, 0.f)))
                {
                    if (SelectKeyById(pendingTimeSelectedId) && HasValidSelection())
                    {
                        state.editTime = pendingTimeValue;
                        ApplyEditorToSelectedKey_TimeOnly();
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();

                if (ImGui::Button(u8"취소", ImVec2(120.f, 0.f)))
                {
                    if (SelectKeyById(pendingTimeSelectedId) && HasValidSelection())
                        SyncEditorFromSelection();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (changedAny && target.player)
            {
                target.player->Invalidate();
                if (!state.recording) target.player->SetTime(state.curTime);
            }
        }

        if (showOrbitArc)
        {
            ImGui::TableSetColumnIndex(1);

            bool changedOrbit = false;

            CamOrbitArcDesc& d = target.sequence->orbitArc;

            auto GetAutoSegment = [&](_vector3& outP0, _vector3& outP1) -> bool
                {
                    const auto& keys = GetKeyFrames();
                    if (keys.size() < 2) return false;

                    int i = state.selectedKeyIdx;
                    if (i < 0 || i >= (int)keys.size()) i = 0;

                    if (i + 1 < (int)keys.size())
                    {
                        outP0 = keys[(size_t)i].pos;
                        outP1 = keys[(size_t)i + 1].pos;
                        return true;
                    }

                    if (i - 1 >= 0)
                    {
                        outP0 = keys[(size_t)i - 1].pos;
                        outP1 = keys[(size_t)i].pos;
                        return true;
                    }

                    return false;
                };

            auto ComputeAxisPerpChord = [&](_vector3 chord) -> _vector3
                {
                    if (chord.LengthSquared() <= 1e-10f) return _vector3(0.f, 1.f, 0.f);

                    chord.Normalize();

                    _vector3 ref(0.f, 1.f, 0.f);
                    const float dd = fabsf(chord.Dot(ref));
                    if (dd > 0.95f) ref = _vector3(0.f, 0.f, 1.f);

                    _vector3 axis = chord.Cross(ref);
                    if (axis.LengthSquared() <= 1e-10f) return _vector3(0.f, 1.f, 0.f);

                    axis.Normalize();
                    return axis;
                };

            auto Label = [&](const char* t)
                {
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextDisabled("%s", t);
                };

            ImGui::SeparatorText("OrbitArc");

            Label("Enabled");
            ImGui::SameLine();
            if (ImGui::Checkbox("##oa_enabled", &d.enabled))
                changedOrbit = true;

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

                    changedOrbit = true;
                }
            }

            ImGui::SameLine();

            if (ImGui::SmallButton("Auto Center##oa_autocenter"))
            {
                _vector3 p0{}, p1{};
                if (GetAutoSegment(p0, p1))
                {
                    d.enabled = true;
                    d.center = (p0 + p1) * 0.5f;
                    changedOrbit = true;
                }
            }

            ImGui::SameLine();

            if (ImGui::SmallButton("Auto Axis##oa_autoaxis"))
            {
                _vector3 p0{}, p1{};
                if (GetAutoSegment(p0, p1))
                {
                    const _vector3 chord = p1 - p0;

                    d.enabled = true;
                    d.axis = ComputeAxisPerpChord(chord);
                    d.NormalizeAxis();

                    changedOrbit = true;
                }
            }

            if (!canAuto) ImGui::EndDisabled();

            ImGui::Spacing();

            const float vecW = 90.f;
            const float gap = 10.f;

            Label("Center");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(vecW);
            if (ImGui::DragFloat("X##oacx", &d.center.x, 0.05f, -99999.f, 99999.f, "%.1f")) changedOrbit = true;
            ImGui::SameLine(0.f, gap);
            ImGui::SetNextItemWidth(vecW);
            if (ImGui::DragFloat("Y##oacy", &d.center.y, 0.05f, -99999.f, 99999.f, "%.1f")) changedOrbit = true;
            ImGui::SameLine(0.f, gap);
            ImGui::SetNextItemWidth(vecW);
            if (ImGui::DragFloat("Z##oacz", &d.center.z, 0.05f, -99999.f, 99999.f, "%.1f")) changedOrbit = true;

            Label("Axis");
            ImGui::SameLine();

            bool axisChanged = false;

            ImGui::SetNextItemWidth(vecW);
            if (ImGui::DragFloat("X##oaax", &d.axis.x, 0.01f, -1.f, 1.f, "%.2f")) axisChanged = true;
            ImGui::SameLine(0.f, gap);
            ImGui::SetNextItemWidth(vecW);
            if (ImGui::DragFloat("Y##oaay", &d.axis.y, 0.01f, -1.f, 1.f, "%.2f")) axisChanged = true;
            ImGui::SameLine(0.f, gap);
            ImGui::SetNextItemWidth(vecW);
            if (ImGui::DragFloat("Z##oaaz", &d.axis.z, 0.01f, -1.f, 1.f, "%.2f")) axisChanged = true;

            if (axisChanged)
            {
                d.NormalizeAxis();
                changedOrbit = true;
            }

            Label("Angle");
            ImGui::SameLine();

            const char* anglePreview = "Shortest";
            if (d.angleMode == CamOrbitArcAngleMode::Longest)  anglePreview = "Longest";
            if (d.angleMode == CamOrbitArcAngleMode::Force180) anglePreview = "Force180";

            ImGui::SetNextItemWidth(160.f);
            if (ImGui::BeginCombo("##oa_angle", anglePreview))
            {
                if (ImGui::Selectable("Shortest", d.angleMode == CamOrbitArcAngleMode::Shortest))
                {
                    d.angleMode = CamOrbitArcAngleMode::Shortest;
                    changedOrbit = true;
                }
                if (ImGui::Selectable("Longest", d.angleMode == CamOrbitArcAngleMode::Longest))
                {
                    d.angleMode = CamOrbitArcAngleMode::Longest;
                    changedOrbit = true;
                }
                if (ImGui::Selectable("Force180", d.angleMode == CamOrbitArcAngleMode::Force180))
                {
                    d.angleMode = CamOrbitArcAngleMode::Force180;
                    changedOrbit = true;
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine(0.f, 12.f);
            Label("CW");
            ImGui::SameLine();
            if (ImGui::Checkbox("##oa_cw", &d.clockwise))
                changedOrbit = true;

            Label("Radius");
            ImGui::SameLine();

            const char* radiusPreview = "Blend";
            if (d.radiusMode == CamOrbitArcRadiusMode::FixedStartRadius) radiusPreview = "Start";
            if (d.radiusMode == CamOrbitArcRadiusMode::FixedEndRadius)   radiusPreview = "End";
            if (d.radiusMode == CamOrbitArcRadiusMode::BlendRadius)      radiusPreview = "Blend";

            ImGui::SetNextItemWidth(160.f);
            if (ImGui::BeginCombo("##oa_radius", radiusPreview))
            {
                if (ImGui::Selectable("Start", d.radiusMode == CamOrbitArcRadiusMode::FixedStartRadius))
                {
                    d.radiusMode = CamOrbitArcRadiusMode::FixedStartRadius;
                    changedOrbit = true;
                }
                if (ImGui::Selectable("End", d.radiusMode == CamOrbitArcRadiusMode::FixedEndRadius))
                {
                    d.radiusMode = CamOrbitArcRadiusMode::FixedEndRadius;
                    changedOrbit = true;
                }
                if (ImGui::Selectable("Blend", d.radiusMode == CamOrbitArcRadiusMode::BlendRadius))
                {
                    d.radiusMode = CamOrbitArcRadiusMode::BlendRadius;
                    changedOrbit = true;
                }
                ImGui::EndCombo();
            }

            if (changedOrbit && target.player)
            {
                target.player->Invalidate();
                if (!state.recording)
                    target.player->SetTime(state.curTime);
            }
        }

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

    ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 colFill = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    ImU32 colTick = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colCursor = ImGui::GetColorU32(ImGuiCol_Text);
    ImU32 colText = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    ImU32 colHot = ImGui::GetColorU32(ImGuiCol_ButtonActive);

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
        ImGui::Text("t = %.1fs", hoverTime);

        const float snapPx = 7.f;
        if (nearest && bestDx <= snapPx)
        {
            ImGui::Separator();
            ImGui::Text("Key #%03u", nearest->keyId);
            ImGui::Text("Time : %.1fs", nearest->time);
            ImGui::Text("Pos  : %.1f, %.1f, %.1f", nearest->pos.x, nearest->pos.y, nearest->pos.z);
            ImGui::Text("Look : %.1f, %.1f, %.1f", nearest->look.x, nearest->look.y, nearest->look.z);
            ImGui::Text("Roll : %.1f", nearest->roll);
            ImGui::Text("FOV  : %.1f", nearest->fov);
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

void CCamPanel::DrawInterpSelector()
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
            if (ImGui::BeginCombo("##ease_preview_combo", Math::GetEaseLabel(selected)))
            {
                for (_uint v = (_uint)EaseType::None; v <= (_uint)EaseType::InBounce; ++v)
                {
                    EaseType e = (EaseType)v;
                    bool isSel = (e == selected);
                    if (ImGui::Selectable(Math::GetEaseLabel(e), isSel))
                        selected = e;
                    if (isSel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            ImGui::TextDisabled(u8"(대각선 = Linear 기준)");

            ImGui::Spacing();

            ImVec2 graphSize(520.f, 220.f);
            DrawEaseGraph(selected, graphSize);

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

    if (open) ImGui::OpenPopup("CameraTool_Help");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"카메라툴 사용법 / Easing 가이드 / 단축키");

    DrawHelpPopup();
    ImGui::Separator();
}

_bool CCamPanel::DrawConstraintBar()
{
    auto GetLabel = [](CamMoveConstraint v) -> const char*
        {
            if (v == CamMoveConstraint::Free)  return "Free";
            if (v == CamMoveConstraint::X)     return "X";
            if (v == CamMoveConstraint::Y)     return "Y";
            if (v == CamMoveConstraint::Z)     return "Z";
            if (v == CamMoveConstraint::XY)    return "XY";
            if (v == CamMoveConstraint::XZ)    return "XZ";
            if (v == CamMoveConstraint::YZ)    return "YZ";
            if (v == CamMoveConstraint::Orbit) return "Orbit";
            return "Free";
        };

    bool changed = false;
    const char* fmt = "%.1f";

    ImGui::PushID("MoveConstraintBar");

    const ImGuiStyle& style = ImGui::GetStyle();
    const float gapS = 4.f;
    const float gapM = 6.f;

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("Constraint");
    ImGui::SameLine(0.f, gapS);

    ImGui::SetNextItemWidth(95.f);
    if (ImGui::BeginCombo("##constraint", GetLabel(state.moveConstraint)))
    {
        auto Pick = [&](CamMoveConstraint v)
            {
                const bool selected = (state.moveConstraint == v);
                if (ImGui::Selectable(GetLabel(v), selected))
                {
                    state.moveConstraint = v;
                    changed = true;
                }
                if (selected) ImGui::SetItemDefaultFocus();
            };

        Pick(CamMoveConstraint::Free);
        Pick(CamMoveConstraint::X);
        Pick(CamMoveConstraint::Y);
        Pick(CamMoveConstraint::Z);
        Pick(CamMoveConstraint::XY);
        Pick(CamMoveConstraint::XZ);
        Pick(CamMoveConstraint::YZ);
        Pick(CamMoveConstraint::Orbit);

        ImGui::EndCombo();
    }

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
    if (state.moveConstraint != CamMoveConstraint::Orbit)
        return false;

    bool changed = false;
    const char* fmt = "%.1f";

    ImGui::PushID("OrbitTargetBar");

    const ImGuiStyle& style = ImGui::GetStyle();
    const float gapS = 4.f;
    const float gapM = 6.f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, style.ItemSpacing.y));

    if (!state.orbit.lookAtCenter)
        ImGui::BeginDisabled();

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("Target");
    ImGui::SameLine(0.f, gapS);

    ImGui::SetNextItemWidth(58.f);
    if (ImGui::DragFloat("##tx", &state.orbit.targetPos.x, 0.1f, -99999.f, 99999.f, fmt)) changed = true;

    ImGui::SameLine(0.f, gapM);

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("Y");
    ImGui::SameLine(0.f, gapS);

    ImGui::SetNextItemWidth(58.f);
    if (ImGui::DragFloat("##ty", &state.orbit.targetPos.y, 0.1f, -99999.f, 99999.f, fmt)) changed = true;

    ImGui::SameLine(0.f, gapM);

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("Z");
    ImGui::SameLine(0.f, gapS);

    ImGui::SetNextItemWidth(58.f);
    if (ImGui::DragFloat("##tz", &state.orbit.targetPos.z, 0.1f, -99999.f, 99999.f, fmt)) changed = true;

    ImGui::SameLine(0.f, gapM);

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("OffY");
    ImGui::SameLine(0.f, gapS);

    ImGui::SetNextItemWidth(58.f);
    if (ImGui::DragFloat("##offy", &state.orbit.offsetY, 0.1f, -99999.f, 99999.f, fmt)) changed = true;

    if (!state.orbit.lookAtCenter)
        ImGui::EndDisabled();

    ImGui::PopStyleVar();
    ImGui::PopID();

    return changed;
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
    MergeNearDuplicateTimes_KeepLast();
    SelectKeyById(newKey.keyId);
    SyncEditorFromSelection();

    if (target.player)
        target.player->Invalidate();
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

        if (target.player)
            target.player->Invalidate();

        return;
    }

    state.selectedKeyIdx = clamp(deleteIdx, 0, (_int)keys.size() - 1);
    SyncEditorFromSelection();

    if (target.player)
        target.player->Invalidate();
}

void CCamPanel::SortKeysByTime_Stable()
{
    auto& keys = GetKeyFrames();
    stable_sort(keys.begin(), keys.end(), [](const CamKeyFrame& a, const CamKeyFrame& b) { return a.time < b.time; });
}

void CCamPanel::MergeNearDuplicateTimes_KeepLast()
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

    if (keys.size() >= 2)
    {
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
                const bool lastIsSelected = (last.keyId == selectedId);
                const bool curIsSelected = (cur.keyId == selectedId);

                if (lastIsSelected && !curIsSelected)
                {
                }
                else if (!lastIsSelected && curIsSelected)
                {
                    last = cur;
                }
                else
                {
                    last = cur;
                }
            }
            else
            {
                merged.push_back(cur);
            }
        }

        keys.swap(merged);
    }

    SelectKeyById(selectedId);

    if (HasValidSelection())
        SyncEditorFromSelection();
    else
        state.selectedKeyIdx = -1;

    if (target.player)
    {
        target.player->Invalidate();
        if (!state.recording)
            target.player->SetTime(state.curTime);
    }
}

void CCamPanel::CaptureSelectedKey_FromCaptureCam()
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