#include "Engine_Defines.h"
#include "InspectorPanel.h"
#include "GUIUtil.h"
#include "GameObject.h"
#include "IInputService.h"

CInspectorPanel::CInspectorPanel(GUI_CONTEXT* context)
	: CBasePanel(context)
{
}

CInspectorPanel::~CInspectorPanel()
{
}

HRESULT CInspectorPanel::Initialize()
{
	return S_OK;
}

void CInspectorPanel::Render_GUI()
{
	ShowObject_Inspector();
}

void CInspectorPanel::Update_Panel(_float dt)
{
	if (m_pContext->pInputDevice->Mouse_Tap(MOUSE_BTN::LB))
	{
		if (!ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
			&& !ImGuizmo::IsOver())
		{
			Set_SelectedObject(nullptr);
		}
	}

	m_pContext->bLocked = m_bLocked;
}

void CInspectorPanel::Set_SelectedObject(CGameObject* objPtr)
{
	if (!m_bLocked)
		m_pContext->pSelectedObject = nullptr;
}

void CInspectorPanel::ShowObject_Inspector()
{
	CGameObject* pObj = m_pContext->pSelectedObject;
	if (!pObj) return;
	ImVec2 windowSize = ImVec2(m_pContext->viewPort.x * 0.15f, (float)m_pContext->viewPort.y);
	ImVec2 windowPos = ImVec2((float)m_pContext->viewPort.x - windowSize.x, 0.0f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	if (ImGui::Begin("Object Inspector", nullptr,   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
	{
		ImGui::PushFontSize(14);
		const char* btnText = m_bLocked ?  " Locked" :  " UnLocked"; 
		if (ImGui::Button(btnText)) {
			m_bLocked = !m_bLocked;
		}
		ImGui::Separator();
		ImGui::PopFontSize();
		pObj->Render_GUI();
	}

	ImGui::End();
}

CInspectorPanel* CInspectorPanel::Create(GUI_CONTEXT* context)
{
	CInspectorPanel* instance = new CInspectorPanel(context);
	if (FAILED(instance->Initialize()))
		Safe_Release(instance);

	return instance;
}

void CInspectorPanel::Free()
{
	__super::Free();
}

