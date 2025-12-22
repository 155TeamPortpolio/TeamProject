#include "pch.h"
#include "EditorPanel.h"
#include "Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "EditorSystem.h"

CEditorPanel::CEditorPanel(GUI_CONTEXT* context)
	:CBasePanel(context)
{
}

void CEditorPanel::Render_GUI()
{
	ImGui::Begin("Editor Panel");
	ImGui::SeparatorText("Add Object");
	if (ImGui::Button("FBX")) {
		CreateObject();
	};
	ImGui::SameLine();
	if (ImGui::Button("Binary")) {
		CreateObject();
	};
	ImGui::SeparatorText("Map Object Option");
	string levelKey = m_pContext->pSelectedLevel->Get_Key();
	_bool isMapLevel = (levelKey == "MapParse_Level");
	if (isMapLevel) {
		if (ImGui::Button("Read Material")) {
			Read_Material();
		};
	}
	ImGui::End();
}

void CEditorPanel::Update_Panel(_float dt)
{
}

void CEditorPanel::CreateObject()
{
	string levelKey = m_pContext->pSelectedLevel->Get_Key();
	auto Model = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_EditModel" }).Build("Parse Obj");
	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(Model, { levelKey,"Model_Layer" });
}

void CEditorPanel::Read_Material()
{
	string folder = Helper::OpenFolder_Dialogue();
	if (folder.empty())
		return;

	CEditorSystem::GetInstance()->Load_MaterialMaps(folder);
}

CEditorPanel* CEditorPanel::Create(GUI_CONTEXT* context)
{
	CEditorPanel* instance = new CEditorPanel(context);
	return instance;
}

void CEditorPanel::Free()
{
	__super::Free();
}
