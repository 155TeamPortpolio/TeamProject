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
		CreateObject(true);
	};

	ImGui::SeparatorText("ImportRules");
	Render_Rules();
	ImGui::End();
}

void CEditorPanel::Update_Panel(_float dt)
{
}

void CEditorPanel::CreateObject(_bool binary)
{
	string levelKey = m_pContext->pSelectedLevel->Get_Key();
	CGameObject* Model = nullptr;
	if (!binary)
		 Model = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_EditModel" }).Build("Parse Obj");
	else
		 Model = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_BinaryModel" }).Build("Binary Obj");

	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(Model, { levelKey,"Model_Layer" });
}

void CEditorPanel::Render_Rules()
{
	auto sys = CEditorSystem::GetInstance();

	ImGui::Separator();
	ImGui::Text("RuleSet");

	// 1) 입력 + Set 버튼을 양쪽으로 벌리기
	const float btnW = 60.0f; // "Set" 버튼 폭
	const float spacing = ImGui::GetStyle().ItemSpacing.x;

	float avail = ImGui::GetContentRegionAvail().x;

	// InputText 폭 = 남은 폭 - 버튼폭 - spacing
	ImGui::SetNextItemWidth(max(1.0f, avail - btnW - spacing));
	ImGui::InputText("##RuleSet", &nowRules);

	ImGui::SameLine(0.0f, spacing);
	// 버튼을 오른쪽 끝에 붙이는 효과 (현재 커서에서 남은 폭 계산)
	// 이미 InputText를 최대한 늘렸으니 그냥 그리면 우측에 딱 붙음.
	if (ImGui::Button("Set", ImVec2(btnW, 0.0f))) {
		sys->Push_NamingRules(nowRules);
		nowRules.clear();
	}

	// 2) 규칙 리스트: 텍스트 왼쪽 / Delete 버튼 오른쪽으로 벌리기
	auto NamingRules = sys->Get_NamingRules();
	for (auto& rule : NamingRules)
	{
		const float delW = 80.0f; // "Delete" 버튼 폭
		float rowAvail = ImGui::GetContentRegionAvail().x;

		// 텍스트 영역 폭을 미리 확보
		ImGui::SetNextItemWidth(max(1.0f, rowAvail - delW - spacing));
		ImGui::TextUnformatted(rule.c_str());

		// 같은 줄에서 버튼을 오른쪽 끝으로 보내기
		ImGui::SameLine(0.0f, spacing);

		// 커서가 텍스트 끝에 오니까, 남은 폭을 계산해서 오른쪽 정렬로 이동
		float remain = ImGui::GetContentRegionAvail().x;
		if (remain > delW) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (remain - delW));

		std::string id = "Delete##" + rule;
		if (ImGui::Button(id.c_str(), ImVec2(delW, 0.0f))) {
			sys->Pop_NamingRules(rule);
		}
	}
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
