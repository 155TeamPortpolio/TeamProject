#include "pch.h"
#include "LogoLevel.h"
#include "GameInstance.h"

/* UI */
#include "UIDirector.h"

CLogoLevel::CLogoLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLogoLevel::Initialize()
{
	return S_OK;
}

HRESULT CLogoLevel::Awake()
{
	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Initialize("Logo_Level");

	CUI_Object* uiObj = Builder::Create_UIObject({ "Logo_Level", "Proto_GameObject_CanvasPanel" })
		.Asset("logo.json")
		.Build("logo");

	uiDirector->Register(uiObj);

	return S_OK;
}

void CLogoLevel::Update()
{
}

HRESULT CLogoLevel::Render()
{
	return S_OK;
}

CLogoLevel* CLogoLevel::Create(const string& LevelKey)
{
	CLogoLevel* instance = new CLogoLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Logo level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CLogoLevel::Free()
{
	__super::Free();

	m_pGameInstance->DestroyInstance();
}