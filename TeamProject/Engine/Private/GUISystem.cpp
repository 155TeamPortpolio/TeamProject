#include "GUISystem.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "IObjectService.h"
#include "IInputService.h"
#include "BasePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "GuizmoPanel.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);                // Use ImGui::GetCurrentContext()

CGUISystem::CGUISystem()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CGUISystem::Initialize(const ENGINE_DESC& engine, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	m_pContext = ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	m_GuiIo = &ImGui::GetIO();

	//		ImFont* fonts = io.Fonts->AddFontFromFileTTF("../../Resources/font/SUIT-Bold.ttf", 16.0f, nullptr,
	//			io.Fonts->GetGlyphRangesKorean()); //
	//		
	//		io.FontDefault = fonts;
	ImGui::StyleColorsDark();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     /*키보드 컨트롤*/
	io.DisplaySize = ImVec2((float)engine.iWinSizeX, (float)engine.iWinSizeY);
	io.IniFilename = nullptr;
	m_tGuiContext.pLevelManager = m_pGameInstance->Get_LevelMgr();
	m_tGuiContext.pObjectManager = m_pGameInstance->Get_ObjectMgr();
	m_tGuiContext.pCameraManager = m_pGameInstance->Get_CameraMgr();
	m_tGuiContext.pInputDevice = m_pGameInstance->Get_InputDev();

	Set_Theme();
	Set_Panel();

	if (!ImGui_ImplWin32_Init(engine.hWnd))
		return E_FAIL;
	if (!ImGui_ImplDX11_Init(pDevice, pContext))
		return E_FAIL;

	return S_OK;
}

void CGUISystem::Update(_float dt)
{
	if (m_pGameInstance->Get_InputDev()->Key_Tap(VK_F9))
		m_bActiveGUI = !m_bActiveGUI;

	m_tGuiContext.viewPort = m_pGameInstance->Get_ClientSize();
	m_GuiIo->DisplaySize = ImVec2(m_tGuiContext.viewPort.x, m_tGuiContext.viewPort.y);

	Adjust_Alpha(dt);

	for (auto& panel : m_Panels) {
		if (panel->Get_Active())
			panel->Update_Panel(dt);
	}
}

void CGUISystem::Set_Theme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.WindowRounding = 4.0f;
	style.FrameRounding = 1.0f;
	style.ScrollbarRounding = 4.0f;
	style.ChildRounding = 4.0f;
	style.GrabRounding = 6.0f;

	style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesToNodes;
	style.TreeLinesSize = 1.5f;        
	style.TreeLinesRounding = 5.0f;      

	style.FramePadding = ImVec2(4, 4);   
	style.ItemSpacing = ImVec2(2, 4);  
	
	style.FrameBorderSize = 1.5f;
	style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.65f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

	style.Colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.80f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);

	style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);

	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

	style.Colors[ImGuiCol_SliderGrab] = style.Colors[ImGuiCol_ChildBg];
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

	style.Colors[ImGuiCol_Tab] = style.Colors[ImGuiCol_ChildBg];
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	style.Colors[ImGuiCol_TreeLines] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
}

void CGUISystem::Set_Panel()
{
	m_Panels.push_back(CHierarchyPanel::Create(&m_tGuiContext));
	m_Panels.push_back(CInspectorPanel::Create(&m_tGuiContext));
	m_Panels.push_back(CGuizmoPanel::Create(&m_tGuiContext));
}

void CGUISystem::Render_Frame()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(500, 5), ImGuiCond_Always);
	ImGui::Begin("FPSWindow", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %d", (int)ImGui::GetIO().Framerate);
	ImGui::End();
}

void CGUISystem::Adjust_Alpha(_float dt)
{
	_bool itemActive = ImGui::IsAnyItemActive();
	_bool hoverWindow = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow); //마우스가 ㅇ올라가면

	if (!itemActive && !hoverWindow)
		m_bUsingUI = false;
	else
		m_bUsingUI = true;

	ImGuiStyle& style = ImGui::GetStyle();
	float alpha = style.Alpha;

	if (m_bUsingUI)
		alpha += dt;
	else
		alpha -= dt * 0.4f;

	alpha = clamp(alpha, 0.3f, 1.f);
	style.Alpha = alpha;

}

void CGUISystem::GUI_Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CGUISystem::Render_GUI()
{
	if (!m_bActiveGUI) return;
	GUI_Begin();

	for (auto& panel : m_Panels) {
		if(panel->Get_Active())
			panel->Render_GUI();
	}
	
	Render_Frame();
#ifdef _USING_GUI
	CGameInstance::GetInstance()->Get_RenderSystem()->Render_GUI();
#endif // _USING_GUI


	GUI_End();
}

ImGuiContext* CGUISystem::GetEngineImGuiContext()
{
	return m_pContext;
}

void CGUISystem::GUI_End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool CGUISystem::Set_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		m_tGuiContext.viewPort = m_pGameInstance->Get_ClientSize();
		m_GuiIo->DisplaySize = ImVec2(m_tGuiContext.viewPort.x, m_tGuiContext.viewPort.y);
		break;
	}

	return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void CGUISystem::Register_Panel(CBasePanel* pPanel)
{
	m_Panels.push_back(pPanel);
}


void CGUISystem::Test()
{
	if (ImGui::Begin("Level Selector")) 
	{
		ImGui::Separator();
		const auto& levelList = CGameInstance::GetInstance()->Get_LevelMgr()->Get_LevelList();
		GUIWidget::ShowListString(levelList, [&](const string& selectedLevel)->void {
			string debugMessage = "Button for '" + selectedLevel + "' was clicked. \n";
			OutputDebugStringA(debugMessage.c_str());
			ImGuiStyle& style = ImGui::GetStyle();
			});

		static _vector vec = XMVectorSet(1, 1, 1, 1);
		vec= GUIWidget::Vector4Float("TestVector", vec);
	}
	ImGui::End(); 
}

CGUISystem* CGUISystem::Create(const ENGINE_DESC& engine, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGUISystem* instance = new CGUISystem();
	if (FAILED(instance->Initialize(engine, pDevice, pContext))) {
		Safe_Release(instance);
	}
	return instance;
}

void CGUISystem::Free()
{
	__super::Free();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	m_pGameInstance->DestroyInstance();

	for (auto& panel : m_Panels)
		Safe_Release(panel);
}
