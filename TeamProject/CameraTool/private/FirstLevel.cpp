#include "pch.h"
#include "FirstLevel.h"
#include "DebugFreeCam.h"
#include "DemoModel.h"
#include "Grid.h"

#include "CamPanel.h"

CFirstLevel::CFirstLevel(const string& key) : CLevel(key)
{
	game = CGameInstance::GetInstance();
	Safe_AddRef(game);
}

HRESULT CFirstLevel::Initialize()
{
	return S_OK;
}

HRESULT CFirstLevel::Awake()
{
	auto proto = game->Get_PrototypeMgr();
	auto objMgr = game->Get_ObjectMgr();
	auto camMgr = game->Get_CameraMgr();

	proto->Add_ProtoType("First_Level", "Proto_GameObject_DebugFreeCam", CDebugFreeCam::Create());
	proto->Add_ProtoType("First_Level", "Proto_GameObject_DemoModel", CDemoModel::Create());
	proto->Add_ProtoType("First_Level", "Proto_GameObject_DemoGrid", CGrid::Create());

	constexpr float kAspect = static_cast<float>(WinX) / WinY;

	CGameObject* debugCamObj = Builder::Create_Object({ "First_Level", "Proto_GameObject_DebugFreeCam" })
		.Camera(kAspect)
		.Position({ 0.f, 3.f, -5.f })
		.Build("DebugFreeCam_Main");

	CGameObject* demoModel = Builder::Create_Object({ "First_Level", "Proto_GameObject_DemoModel" })
		.Position({})
		.Build("Demo_Model");
	demoModel->Get_Component<CTransform>()->Scale({ 0.5f, 0.5f, 0.5f });

	CGameObject* demoGrid = Builder::Create_Object({ "First_Level", "Proto_GameObject_DemoGrid" })
		.Position({ 0, 0, 0 })
		.Scale({ 50.f, 1.f, 50.f })
		.Build("Demo_Grid");

	objMgr->Add_Object(debugCamObj, { "First_Level", "Camera_Layer" });
	objMgr->Add_Object(demoModel, { "First_Level", "Model_Layer" });
	objMgr->Add_Object(demoGrid, { "First_Level", "Grid_Layer" });

	camMgr->Set_MainCam(debugCamObj->Get_Component<CCamera>());

	auto guiSys = game->Get_GUISystem();
	auto camPanel = CCamPanel::Create(guiSys->Get_Context());
	camPanel->SetCaptureTarget(static_cast<CCamObj*>(debugCamObj));
	
	OBJECT_HANDLE objHandle = demoModel->Get_Handle();
	camPanel->SetSpaceReference(objHandle);
	guiSys->Register_Panel(camPanel);
	return S_OK;
}

void CFirstLevel::Update()
{

}

HRESULT CFirstLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("첫 레벨입니다."));
	return S_OK;
}

CFirstLevel* CFirstLevel::Create(const string& key)
{
	auto inst = new CFirstLevel(key);
	inst->Initialize();
	return inst;
}

void CFirstLevel::Free()
{
	__super::Free();
	Safe_Release(game);
}