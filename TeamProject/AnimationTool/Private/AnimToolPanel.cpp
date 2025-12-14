#include "AnimToolPanel.h"
#include "Helper_Func.h"
#include "GameInstance.h"

CAnimToolPanel::CAnimToolPanel(GUI_CONTEXT* pContext)
	: CBasePanel{pContext}
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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

	if (ImGui::Button("Load Clips")) {
		Load_Clips();
	}
	
	ImGui::SameLine();

	ImGui::EndChild();

	__super::Render_GUI();
}

void CAnimToolPanel::Load_Clips()
{
	vector<string> files = Helper::OpenMultiFiles();
	for (auto& path : files) {
		m_
	}
}

CBasePanel* CAnimToolPanel::Create(GUI_CONTEXT* context)
{
	return new CAnimToolPanel(context);
}

void CAnimToolPanel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
