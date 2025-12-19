#include "Engine_Defines.h"
#include "GuizmoPanel.h"
#include "ICameraService.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

CGuizmoPanel::CGuizmoPanel(GUI_CONTEXT* context)
	:CBasePanel{ context }
{
}

CGuizmoPanel::~CGuizmoPanel()
{
}

HRESULT CGuizmoPanel::Initialize()
{
	gizmoOperation = ImGuizmo::ROTATE;
	return S_OK;
}

void CGuizmoPanel::Render_GUI()
{
    if (!m_pContext->pSelectedObject)
        return;

    ImVec2 windowPos = ImVec2((float)m_pContext->viewPort.x - 450.0f, 0.0f);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin("Gizmo Control", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);

    const ImVec4 onBtn = ImVec4(0.20f, 0.70f, 0.25f, 1.00f);
    const ImVec4 onHovered = ImVec4(0.25f, 0.80f, 0.30f, 1.00f);
    const ImVec4 onActive = ImVec4(0.15f, 0.60f, 0.20f, 1.00f);

    const ImVec4 offBtn = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    const ImVec4 offHovered = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    const ImVec4 offActive = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    const bool isOn = m_isActive;

    ImGui::PushStyleColor(ImGuiCol_Button, isOn ? onBtn : offBtn);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isOn ? onHovered : offHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, isOn ? onActive : offActive);

    if (ImGui::Button(isOn ? "Gizmo Active: ON" : "Gizmo Active: OFF"))
        m_isActive = !m_isActive;

    ImGui::PopStyleColor(3);

    auto PushOpStyle = [](bool selected)
        {
            if (!selected) return;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.45f, 0.90f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.55f, 0.95f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.40f, 0.85f, 1.00f));
        };
    auto PopOpStyle = [](bool selected)
        {
            if (selected) ImGui::PopStyleColor(3);
        };

    const bool selT = (gizmoOperation == ImGuizmo::TRANSLATE);
    const bool selR = (gizmoOperation == ImGuizmo::ROTATE);
    const bool selS = (gizmoOperation == ImGuizmo::SCALE);

    PushOpStyle(selT);
    if (ImGui::Button("Translate")) gizmoOperation = ImGuizmo::TRANSLATE;
    PopOpStyle(selT);

    ImGui::SameLine();

    PushOpStyle(selR);
    if (ImGui::Button("Rotate")) gizmoOperation = ImGuizmo::ROTATE;
    PopOpStyle(selR);

    ImGui::SameLine();

    PushOpStyle(selS);
    if (ImGui::Button("Scale")) gizmoOperation = ImGuizmo::SCALE;
    PopOpStyle(selS);

    if (m_isActive)
        ShowObject_Guizmo();

    ImGui::End();
}


void CGuizmoPanel::ShowObject_Guizmo()
{
	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

	ImGuizmo::SetRect(
		static_cast<float>(0),
		static_cast<float>(0),
		static_cast<float>(m_pContext->viewPort.x),
		static_cast<float>(m_pContext->viewPort.y)
	);

	CTransform* objTransform = m_pContext->pSelectedObject->Get_Component<CTransform>();
	_float4x4 temp = *objTransform->Get_WorldMatrix_Ptr();

	ImGuizmo::Manipulate(
		(float*)m_pContext->pCameraManager->Get_ViewMatrix(),
		(float*)m_pContext->pCameraManager->Get_ProjMatrix(),
		gizmoOperation,
		ImGuizmo::WORLD,
		(float*)&temp
	);

	if (ImGuizmo::IsUsing())
		objTransform->TranslateMatrix(XMLoadFloat4x4(&temp));
}


CGuizmoPanel* CGuizmoPanel::Create(GUI_CONTEXT* context)
{
	CGuizmoPanel* instance = new CGuizmoPanel(context);
	if (FAILED(instance->Initialize()))
		Safe_Release(instance);

	return instance;
}

void CGuizmoPanel::Free()
{
	__super::Free();
}
