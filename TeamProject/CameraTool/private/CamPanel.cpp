#include "pch.h"
#include "CamPanel.h"

void CamPanel::Init()
{
}

void CamPanel::Update_Panel(_float dt)
{
}

void CamPanel::Render_GUI()
{
    constexpr float leftW  = 200.0f;  // Hierarchy Æø
    constexpr float rightW = 250.0f;  // Inspector Æø
    constexpr float height = 220.0f;  // CamPanel ³ôÀÌ

    ImVec2 pos(leftW, (float)m_pContext->viewPort.y - height);
    ImVec2 size((float)m_pContext->viewPort.x - leftW - rightW, height);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Camera Tool", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
    {
        DrawToolbar();    
        ImGui::Separator();

        DrawCamSelector();
        ImGui::Separator();

        DrawKeyframeArea();   
        ImGui::Separator();

        DrawTimeline();      
    }
    ImGui::End();
}

void CamPanel::DrawToolbar()
{
    if (ImGui::Button("Record")) { recording = !recording; }
    ImGui::SameLine();
    if (ImGui::Button("Play")) { playing = true; }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) { playing = false; curTime = 0.f; }
    ImGui::SameLine();
    ImGui::Checkbox("Loop", &loop);
}

void CamPanel::DrawCamSelector()
{
    ImGui::Text("Target Camera/Track:");
    ImGui::SameLine();
    ImGui::TextDisabled("(combo here)");
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
    if (ImGui::Button("+ Add")) {}
    ImGui::SameLine();
    if (ImGui::Button("- Delete")) {}
    ImGui::SeparatorText("Keyframes");
    ImGui::TextDisabled("(list here)");
}

void CamPanel::DrawKeyframeEditor()
{
    ImGui::SeparatorText("Selected Key");
    ImGui::TextDisabled("(pos / lookDir / roll / fov / time editors here)");
}

void CamPanel::DrawTimeline()
{
    ImGui::Text("Time");
    ImGui::SameLine();
    ImGui::SliderFloat("##scrub", &curTime, 0.f, endTime);

    ImGui::SameLine();
    ImGui::SliderFloat("Zoom", &zoom, 0.25f, 5.0f);
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