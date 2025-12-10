#include "HierarchyPanel.h"
#include "GUIWidget.h"
#include "GameInstance.h"

#include "ILevelService.h"
#include "IObjectService.h"

#include "Layer.h"
#include "GameObject.h"

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
	
	float fWincY = (float)m_pContext->viewPort.y;
	float fPanelCX = 200;


	ImGui::SetNextWindowPos(ImVec2(m_fPosX, 0));
	ImGui::SetNextWindowSize(ImVec2(fPanelCX, fWincY));
	ImGui::Begin("##Hierachy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ShowLevelList();
	ShowLayerList(nowLevel);
	ShowObjectList();
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowPos(ImVec2(m_fPosX + fPanelCX, 0));
	ImGui::Begin("##HierachyBtn", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoDecoration);
	if (ImGui::ArrowButton("##HierachyOpenBtn",m_bOpened ? ImGuiDir::ImGuiDir_Left : ImGuiDir::ImGuiDir_Right)) {
		m_bOpened = !m_bOpened;
	}
	ImGui::End();
	ImGui::PopStyleVar(1);
}

void CHierarchyPanel::Update_Panel(_float dt)
{
	if (m_bOpened) {
		if (m_fPosX < 0)
			m_fPosX += dt * 250;
		else
			m_fPosX = 0;
	}
	else {
		if (m_fPosX > -200)
			m_fPosX -= dt * 250;
		else
			m_fPosX = -200;
	}
}

void CHierarchyPanel::ShowLevelList()
{
	const vector<string>& levelList = m_pContext->pLevelManager->Get_LevelList();

	const string& nowLevelKey = m_pContext->pLevelManager->Get_NowLevelKey();

	auto it = std::find(levelList.begin(), levelList.end(), nowLevelKey);
	if (it != levelList.end())
		m_iSelectedLevel = distance(levelList.begin(), it);

	GUIWidget::ShowCombo(levelList, m_iSelectedLevel, "LevelList", [&](_uint ID) {
		if (m_iSelectedLevel == ID)
			return;
		if (levelList[ID] == G_GlobalLevelKey)
			return;
		m_iSelectedLevel = ID;
		m_pContext->pLevelManager->Request_ChangeLevel(levelList[m_iSelectedLevel]);
		}
	);
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

	GUIWidget::ShowCombo(layers, m_iSelectedLayer, "LayerList", [&](_uint ID) {m_iSelectedLayer = ID; });

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

	for (auto& Object : ObjectVector) {
		if (!Object || !Object->Is_Root()) continue;
		bool isSel = (m_pContext->pSelectedObject == Object);
		Object->RenderHierarchy(m_pContext->pSelectedObject, isSel);
	}
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

