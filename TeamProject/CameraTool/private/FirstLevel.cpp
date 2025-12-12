#include "pch.h"
#include "FirstLevel.h"
#include "DebugFreeCam.h"

FirstLevel::FirstLevel(const string& key) : CLevel(key)
{
	game = CGameInstance::GetInstance();
	Safe_AddRef(game);
}

HRESULT FirstLevel::Initialize()
{
	return S_OK;
}

HRESULT FirstLevel::Awake()
{
	auto proto  = game->Get_PrototypeMgr();
	auto objMgr = game->Get_ObjectMgr();
	auto camMgr = game->Get_CameraMgr();

	proto->Add_ProtoType("First_Level", "Proto_GameObject_DebugFreeCam", DebugFreeCam::Create());

	constexpr float aspect = static_cast<float>(WinX) / WinY;

	CGameObject* debugCamObj = Builder::Create_Object({ "First_Level", "Proto_GameObject_DebugFreeCam" })
		.Camera({ aspect })
		.Position({ 0.f, 3.f, -5.f })
		.Build("DebugFreeCam_Main");

	objMgr->Add_Object(debugCamObj, { "First_Level", "Camera_Layer" });

	camMgr->Set_MainCam(debugCamObj->Get_Component<CCamera>());

	return S_OK;
}

void FirstLevel::Update()
{

}

HRESULT FirstLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("첫 레벨입니다."));
	return S_OK;
}

FirstLevel* FirstLevel::Create(const string& key)
{
	auto inst = new FirstLevel(key);
	inst->Initialize();
	return inst;
}

void FirstLevel::Free()
{
	__super::Free();
	Safe_Release(game);
}