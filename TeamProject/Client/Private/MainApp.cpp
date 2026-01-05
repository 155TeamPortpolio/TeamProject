#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "IResourceService.h"

#include "TestLevel.h"
#include "LogoLevel.h"
#include "LoadingLevel.h"

#include "UIDirector.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	ENGINE_DESC desc{};

	desc.hWnd = g_hWnd;
	desc.eWinMode = WINMODE::WIN;
	desc.iWinSizeX = g_iWinSizeX;
	desc.iWinSizeY = g_iWinSizeY;

	if (m_pGameInstance->Init_Engine(desc)) {
		m_pDevice = m_pGameInstance->Get_Device();
		m_pDeviceContext = m_pGameInstance->Get_Context();
	}
	
	Set_Levels();

	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Initialize();

	#ifdef  _USING_GUI
		ImGui::SetCurrentContext(m_pGameInstance->Get_GUISystem()->GetEngineImGuiContext());
	#endif //  _USING_GUI
	
	return S_OK;
}

void CMainApp::Update(const float dt)
{
	m_pGameInstance->Update_Engine(dt);
}


HRESULT CMainApp::Render()
{
	_float4 color = { 0.f,0.f,0.f,1.f };
	m_pGameInstance->Draw_Begin(&color);
	m_pGameInstance->Draw();
	m_pGameInstance->Draw_End();
	return S_OK;
}

void CMainApp::Set_Levels() //레벨 등록 함수 ->등록 끝내면
{
	m_pGameInstance->Get_LevelMgr()->Register_Level("Test_Level", []()->CLevel* {return CTestLevel::Create("Test_Level"); });
	m_pGameInstance->Get_LevelMgr()->Register_Level("Logo_Level", []()->CLevel* {return CLogoLevel::Create("Logo_Level"); });
	m_pGameInstance->Get_LevelMgr()->Register_Level("Loading_Level", []()->CLevel* {return CLoadingLevel::Create("Loading_Level"); });

	m_pGameInstance->Notify_LevelSet(); 
	m_pGameInstance->Get_LevelMgr()->Request_ChangeLevel("Test_Level",false); 
} 

CMainApp* CMainApp::Create()
{
	CMainApp* instance = new CMainApp();

	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

void CMainApp::Free()
{
	__super::Free();
	m_pGameInstance->Release_Engine();
	m_pGameInstance->DestroyInstance();
	CUIDirector::GetInstance()->DestroyInstance();
}

