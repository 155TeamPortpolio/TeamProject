#include "pch.h"
#include "Scott_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"

CScott_Level::CScott_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CScott_Level::Initialize()
{
	return S_OK;
}

HRESULT CScott_Level::Awake()
{
	return S_OK;
}

void CScott_Level::Update()
{
	
}

HRESULT CScott_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CScott_Level::PreLoad_Level()
{
}

CScott_Level* CScott_Level::Create(const string& LevelKey)
{
	CScott_Level* instance = new CScott_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CScott_Level::Free()
{
	__super::Free();
	m_pGameInstance->DestroyInstance();
}
