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
		Extract_Clip();
	}

	ImGui::SameLine();

	if (ImGui::Button("Material Loadar")) {
		
	}
	ImGui::EndChild();

	__super::Render_GUI();
}

void CAnimToolPanel::Extract_Clip()
{
	m_Importer.FreeScene();

	string path = Helper::OpenFile_Dialogue();
	unsigned int iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	m_pAIScene = m_Importer.ReadFile(path.c_str(), iFlag);
	 
	if (nullptr == m_pAIScene)
		return;

	if (!m_pAIScene->HasAnimations())
		return;

	int iNumAnim = m_pAIScene->mNumAnimations;

	for (int i = 0; i < iNumAnim; i++) {

	}
}

CBasePanel* CAnimToolPanel::Create(GUI_CONTEXT* context)
{
	return new CAnimToolPanel(context);
}

void CAnimToolPanel::Free()
{
	__super::Free();
	m_Importer.FreeScene();
}
