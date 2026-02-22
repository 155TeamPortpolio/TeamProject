#include "pch.h"
#include "LogoLevel.h"
#include "GameInstance.h"
#include "LevelMgr.h"

/*Video*/
#include "UI_LogoVideo.h"

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
	uiDirector->Load_LevelObjects("Logo_Level");

	Create_LogoVideo();
	return S_OK;
}

void CLogoLevel::Update()
{
	if (InputDevice()->Key_Tap('Y')) {
		LevelManager()->Request_ChangeLevel("Scott_Level", true);
	}
}

HRESULT CLogoLevel::Render()
{
	return S_OK;
}

HRESULT CLogoLevel::Create_LogoVideo()
{
	PrototypeManager()->Add_ProtoType("Logo_Level", "Proto_GameObject_LogoVideo", CUI_LogoVideo::Create());

	auto pObj = Builder::Create_UIObject({ "Logo_Level", "Proto_GameObject_LogoVideo" })
		.Build("video");

	UIManager()->Add_UIObject(pObj, "Logo_Level");

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