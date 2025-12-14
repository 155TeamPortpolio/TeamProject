#include "pch.h"
#include "MapToolGui.h"

CMapToolGui::CMapToolGui(GUI_CONTEXT* pContext)
	: CBasePanel(pContext)
{
}

void CMapToolGui::Update_Panel(_float dt)
{
}

void CMapToolGui::Render_GUI()
{
	ImGui::PushID(this);

	ImGui::SeparatorText("Transform");



	ImGui::PopID();
}

CMapToolGui* CMapToolGui::Create(GUI_CONTEXT* pContext)
{
	return new CMapToolGui(pContext);
}

void CMapToolGui::Free()
{
	__super::Free();
}
