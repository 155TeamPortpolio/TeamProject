#include "pch.h"
#include "MainApp.h"
#include "FirstLevel.h"

void MainApp::Init()
{
	game = CGameInstance::GetInstance();
	Safe_AddRef(game);

	ENGINE_DESC desc{};
	desc.hWnd      = g_hWnd;
	desc.eWinMode  = WINMODE::WIN;
	desc.iWinSizeX = WinX;
	desc.iWinSizeY = WinY;

	game->Init_Engine(desc);
	device  = game->Get_Device();
	context = game->Get_Context();

	Set_Levels();

#ifdef  _USING_GUI
	ImGui::SetCurrentContext(game->Get_GUISystem()->GetEngineImGuiContext());
#endif 
}

void MainApp::Update(float dt)
{
	game->Update_Engine(dt);
}

void MainApp::Render()
{
	_float4 color = { 0.f, 0.f, 0.f, 1.f };
	game->Draw_Begin(&color);
	game->Draw();
	game->Draw_End();
}

void MainApp::Set_Levels()
{
	game->Get_LevelMgr()->Register_Level("First_Level", []()-> CLevel* { return FirstLevel::Create("First_Level"); });
	game->Notify_LevelSet();
	game->Get_LevelMgr()->Request_ChangeLevel("First_Level", false);
}

MainApp* MainApp::Create()
{
	auto inst = new MainApp();
	inst->Init();
	return inst;
}

void MainApp::Free()
{
	__super::Free();
	game->Release_Engine();
	game->DestroyInstance();
}