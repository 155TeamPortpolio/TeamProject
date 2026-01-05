#include "pch.h"
#include "LoadingLevel.h"
#include "GameInstance.h"

/* UI */
#include "UIDirector.h"

CLoadingLevel::CLoadingLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLoadingLevel::Initialize()
{
	return S_OK;
}

HRESULT CLoadingLevel::Awake()
{
	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Loading_Level");

	return S_OK;
}

void CLoadingLevel::Update()
{
}

HRESULT CLoadingLevel::Render()
{
	return S_OK;
}

CLoadingLevel* CLoadingLevel::Create(const string& LevelKey)
{
	CLoadingLevel* instance = new CLoadingLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Laoding level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CLoadingLevel::Free()
{
	__super::Free();

	m_pGameInstance->DestroyInstance();
}