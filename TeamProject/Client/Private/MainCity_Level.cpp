#include "pch.h"
#include "MainCity_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"

CMainCity_Level::CMainCity_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainCity_Level::Initialize()
{
	return S_OK;
}

HRESULT CMainCity_Level::Awake()
{
	return S_OK;
}

void CMainCity_Level::Update()
{
	
}

HRESULT CMainCity_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CMainCity_Level::PreLoad_Level()
{
}

CMainCity_Level* CMainCity_Level::Create(const string& LevelKey)
{
	CMainCity_Level* instance = new CMainCity_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CMainCity_Level::Free()
{
	__super::Free();
	m_pGameInstance->DestroyInstance();
}
