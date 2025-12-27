#include "pch.h"
#include "Engine_Defines.h"

#include "MainApp.h"
#include "GameInstance.h"
#include "ModelEditLevel.h"
#include "MapParseLevel.h"

#include "EditorPanel.h"
#include "FreeCam.h"
#include "EditModel.h"
#include "BinaryModel.h"
#include "EditorSystem.h"

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

#ifdef  _USING_GUI
	ImGui::SetCurrentContext(m_pGameInstance->Get_GUISystem()->GetEngineImGuiContext());
#endif //  _USING_GUI

	auto panel = CEditorPanel::Create(m_pGameInstance->Get_GUISystem()->Get_Context());
	m_pGameInstance->Get_GUISystem()->Register_Panel(panel);
	ReadyBase();

	return S_OK;
}

void CMainApp::Update(const float dt)
{
	m_pGameInstance->Update_Engine(dt);
	m_pSystem->Update();
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
	m_pGameInstance->Get_LevelMgr()->Register_Level("ModelEdit_Level", []()->CLevel* {return CModelEditLevel::Create("ModelEdit_Level"); });
	m_pGameInstance->Get_LevelMgr()->Register_Level("MapParse_Level", []()->CLevel* {return CMapParseLevel::Create("MapParse_Level"); });
	m_pGameInstance->Notify_LevelSet();
	m_pGameInstance->Get_LevelMgr()->Request_ChangeLevel("ModelEdit_Level", false); //모델에디터 레벨로 시작!
}

void CMainApp::ReadyBase()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EditCamera", CFreeCam::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EditModel", CEditModel::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BinaryModel", CBinaryModel::Create());

	m_pSystem = CEditorSystem::GetInstance();

	if (m_pSystem)
		m_pSystem->Initialize();

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
	Safe_Release(m_pSystem);
}

