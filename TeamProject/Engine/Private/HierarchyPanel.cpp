#include "Engine_Defines.h"
#include "HierarchyPanel.h"
#include "GUIUtil.h"
#include "GameInstance.h"

#include "ILevelService.h"
#include "IObjectService.h"

#include "Layer.h"
#include "GameObject.h"
#include "UI_Object.h"
#include "Level.h"

CHierarchyPanel::CHierarchyPanel(GUI_CONTEXT* context)
	:CBasePanel(context)
{
}

CHierarchyPanel::~CHierarchyPanel()
{
}

HRESULT CHierarchyPanel::Initialize()
{

	return S_OK;
}
void CHierarchyPanel::Render_GUI()
{
	const string& nowLevel = m_pContext->pLevelManager->Get_NowLevelKey();

	const float winH = (float)m_pContext->viewPort.y;
	m_fPanelCX = 200.0f;

	// 1) 메인 패널
	ImGuiWindowFlags panelFlags =
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos(ImVec2(m_fPosX, 0.f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_fPanelCX, winH), ImGuiCond_Always);

	ImGui::Begin("##Hierachy", nullptr, panelFlags);

	ShowLevelList();
	ShowLayerList(nowLevel);

	ImGui::SeparatorText("Show Object Type");
	const char* label = m_bShowUI ? "UIObject" : "GameObject";

	// 왼쪽 라벨
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label);

	// 오른쪽 토글 (같은 줄 우측 정렬)
	const float toggleWidth = ImGui::GetFrameHeight() * 1.55f;
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	float startX = ImGui::GetCursorPosX();
	float avail = ImGui::GetContentRegionAvail().x;

	ImGui::SameLine(startX + avail - toggleWidth - spacing);
	ToggleButton("##objToggle", &m_bShowUI);

	if (!m_bShowUI) ShowObjectList();
	else           ShowUIObjectList();

	ImGui::End();

	// 2) 옆에 붙는 버튼(화살표)
	ImGuiWindowFlags btnFlags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	ImGui::SetNextWindowPos(ImVec2(m_fPosX + m_fPanelCX, 0.f), ImGuiCond_Always);
	ImGui::Begin("##HierachyBtn", nullptr, btnFlags);

	if (ImGui::ArrowButton("##HierachyOpenBtn", m_bOpened ? ImGuiDir_Left : ImGuiDir_Right))
		m_bOpened = !m_bOpened;

	ImGui::End();
	ImGui::PopStyleVar();
}

void CHierarchyPanel::Update_Panel(_float dt)
{
	const float speed = 250.0f;
	const float openX = 0.0f;
	const float closeX = -m_fPanelCX;

	if (m_bOpened)  m_fPosX += dt * speed;
	else            m_fPosX -= dt * speed;

	if (m_fPosX > openX)  m_fPosX = openX;
	if (m_fPosX < closeX) m_fPosX = closeX;
}


void CHierarchyPanel::ShowLevelList()
{
	const vector<string>& levelList = m_pContext->pLevelManager->Get_LevelList();

	const string& nowLevelKey = m_pContext->pLevelManager->Get_NowLevelKey();

	auto it = std::find(levelList.begin(), levelList.end(), nowLevelKey);
	if (it != levelList.end())
		m_iSelectedLevel = distance(levelList.begin(), it);

	GuiUtil::ShowCombo(levelList, m_iSelectedLevel, "LevelList", [&](_uint ID) {
		if (m_iSelectedLevel == ID)
			return;
		if (levelList[ID] == G_GlobalLevelKey)
			return;
		m_iSelectedLevel = ID;
		m_pContext->pLevelManager->Request_ChangeLevel(levelList[m_iSelectedLevel]);
		}
	);
	m_pContext->pSelectedLevel = m_pContext->pLevelManager->Get_CurrentLevel();
}

void CHierarchyPanel::ShowLayerList(const string& nowLevel)
{
	const auto& LayerMap = m_pContext->pObjectManager->Get_LevelLayer(nowLevel);
	vector<string> layers;

	for (auto& pair : LayerMap)
		layers.push_back(pair.first);

	if (layers.empty())
		return;

	if (layers[m_iSelectedLayer] == G_GlobalLevelKey) return;

	GuiUtil::ShowCombo(layers, m_iSelectedLayer, "LayerList", [&](_uint ID) {m_iSelectedLayer = ID; });

	auto iter = LayerMap.find(layers[m_iSelectedLayer]);

	if (iter != LayerMap.end())
		m_pContext->pSelectedLayer = iter->second;
}

void CHierarchyPanel::ShowObjectList()
{
	if (!m_pContext->pSelectedLayer) return;
	auto& ObjectVector = m_pContext->pSelectedLayer->Get_AllObject();


	string InstanceListHeader = "Instances ("  + to_string(ObjectVector.size()) + " )";
	ImGui::SeparatorText(InstanceListHeader.c_str());

	if (ObjectVector.empty())
		return;

	for (auto& Object : ObjectVector) {
		if (!Object || !Object->Is_Root()) continue;
		bool isSel = (m_pContext->pSelectedObject == Object);
		Object->RenderHierarchy(m_pContext->pSelectedObject, isSel);
	}
}

void CHierarchyPanel::ShowUIObjectList()
{
	if (!m_pContext->pSelectedLevel) return;
	auto& LevelObj = m_pContext->pUIManager->Get_LevelUI(m_pContext->pSelectedLevel->Get_Key());

	if (LevelObj.empty())
		return;

	string InstanceListHeader = "Instances (" + to_string(LevelObj.size()) + " )";
	ImGui::SeparatorText(InstanceListHeader.c_str());

	for (auto& Object : LevelObj) {
		if (!Object || !Object->Is_Root()) continue;
		bool isSel = (m_pContext->pSelectedObject == Object);
		Object->RenderHierarchy(m_pContext->pSelectedObject, isSel);
	}
}

_bool CHierarchyPanel::ToggleButton(const char* str_id, _bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	_float height = ImGui::GetFrameHeight();
	_float width = height * 1.55f;
	_float radius = height * 0.5f;

	// InvisibleButton: 클릭 영역만 잡아줌
	if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
		*v = !*v;

	// 색 가져오기
	ImU32 col_bg = ImGui::GetColorU32(*v ? ImGuiCol_ButtonActive : ImGuiCol_FrameBg);

	// 바탕(타원)
	draw_list->AddRectFilled(
		p,
		ImVec2(p.x + width, p.y + height),
		col_bg,
		radius
	);

	// 동그라미 위치
	float t = *v ? 1.0f : 0.0f;
	float cx = p.x + radius + t * (width - 2 * radius);

	draw_list->AddCircleFilled(ImVec2(cx, p.y + radius),
		radius - 2.0f,
		IM_COL32(255, 255, 255, 255));

	return *v;
}


vector<string> CHierarchyPanel::ConvertObjectNameList(CLayer* layer)
{
	vector<string> vectorName;

	for (CGameObject* obj : layer->Get_AllObject()) {
		vectorName.push_back(obj->Get_InstanceName());
	}
	return vectorName;
}

CHierarchyPanel* CHierarchyPanel::Create(GUI_CONTEXT* context)
{
	CHierarchyPanel* instance = new CHierarchyPanel(context);
	if (FAILED(instance->Initialize()))
	{
		Safe_Release(instance);
	}
	return instance;
}

void CHierarchyPanel::Free()
{
	__super::Free();
}

