#include "GuizmoPanel.h"
#include "ICameraService.h"
#include "GameObject.h"
#include "Transform.h"

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
	ImVec2 windowPos = ImVec2((float)m_pContext->viewPort.x - 450, 0);
	ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Once);

	ImGui::Begin("Gizmo Control",0,ImGuiWindowFlags_NoDecoration| ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);

	if (ImGui::Button("Translate"))
		gizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::Button("Rotate"))
		gizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::Button("Scale"))
		gizmoOperation = ImGuizmo::SCALE;

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
