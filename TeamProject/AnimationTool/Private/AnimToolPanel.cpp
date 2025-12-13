#include "AnimToolPanel.h"
#include "Helper_Func.h"

CAnimToolPanel::CAnimToolPanel(GUI_CONTEXT* pContext)
	: CBasePanel{pContext}
{
}

void CAnimToolPanel::Update_Panel(_float dt)
{
}

void CAnimToolPanel::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::SeparatorText("Model Load");
	ImGui::BeginChild("##Loaded OBJECT BTN", ImVec2{ 0, childHeight }, true);

	if (ImGui::Button("Model Load")) {
		string path = Helper::OpenFile_Dialogue();
	}

	ImGui::SameLine();

	if (ImGui::Button("Material Loadar")) {
		
	}
	ImGui::EndChild();

	__super::Render_GUI();
}

CBasePanel* CAnimToolPanel::Create(GUI_CONTEXT* context)
{
	return new CAnimToolPanel(context);
}


void CAnimToolPanel::Free()
{
	__super::Free();
}
