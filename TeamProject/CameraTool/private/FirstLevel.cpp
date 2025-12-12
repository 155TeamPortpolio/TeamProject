#include "pch.h"
#include "FirstLevel.h"

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
	auto proto = game->Get_PrototypeMgr();

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