#include "pch.h"
#include "MainCity_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"

// Camera
#include "Camera.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "ShadowCam.h"

/* MapData */
#include "MapLoader.h"
#include "MapPlacedObject.h"
#include "MapTriggerObject.h"

/* UI */
#include "UIDirector.h"

CMainCity_Level::CMainCity_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainCity_Level::Initialize()
{
	return S_OK;
}

HRESULT CMainCity_Level::Awake()
{
	//============== Map ============================
	PrototypeManager()->Add_ProtoType("MainCity_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());
	PrototypeManager()->Add_ProtoType("MainCity_Level", "Proto_GameObject_MapTriggerObject", CMapTriggerObject::Create());

	//============== Map ============================
	ReadyMap();
	CUIDirector::GetInstance()->Load_LevelObjects("MainCity_Level");
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

void CMainCity_Level::ReadyMap()
{
	//CMapLoader* pMapLoader = CMapLoader::Create(m_LevelKey, m_pMapDataCloud, "TrainingRoom");
	//if (nullptr == pMapLoader)
	//	MSG_BOX("Failed to Load MapData!");
	//Safe_Release(pMapLoader);
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
