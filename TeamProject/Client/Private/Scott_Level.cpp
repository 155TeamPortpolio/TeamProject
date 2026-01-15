#include "pch.h"
#include "Scott_Level.h"
#include "Helper_Func.h"
#include "GameInstance.h"

/* MapData */
#include "MapLoader.h"
#include "MapPlacedObject.h"
#include "MapTriggerObject.h"

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
	//============== Map ============================
	Ready_Map("Test_Level", "Zero_Worksite");

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

void CScott_Level::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	//// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();
	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
}

void CScott_Level::Rake_MapResources()
{
	filesystem::path MapDataFolderPath = "../Bin/Resources/MapData/Model/";
	Helper::EnsureDirectoryExist(MapDataFolderPath);


	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	for (const auto& entry : filesystem::recursive_directory_iterator(MapDataFolderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".model")
		{
			filesystem::path ModelPath = entry.path();
			filesystem::path MaterialPath = ModelPath;
			MaterialPath.replace_extension(".mat");


			pRcsMgr->Add_ResourcePath(ModelPath.filename().string(), ModelPath.string());
			pRcsMgr->Add_ResourcePath(MaterialPath.filename().string(), MaterialPath.string());

		}
	}
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
