#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCam.h"
#include "TestObject.h"

#include "Camera.h"

/* MapData */
#include "MapLoader.h"
#include "MapPlacedObject.h"

CTestLevel::CTestLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTestLevel::Initialize()
{



	return S_OK;
}

HRESULT CTestLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();

	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_FreeCam", CFreeCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());

	// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();

	//경로는 임시. 추후 로직 변경예정
	CMapLoader* pMapLoader = CMapLoader::Create("Test_Level",  "../Bin/Resources/MapData/Data/MapTool.Data_1_20251217_200942.json" );
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	// 바로 지움
	Safe_Release(pMapLoader);

	Ready_Camera();
	return S_OK;
}

void CTestLevel::Update()
{
}

void CTestLevel::Ready_Camera()
{
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CGameObject* Camera =
		Builder::Create_Object({ "Test_Level" ,"Proto_GameObject_FreeCam" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0.f, 3.f, -3.f })
		.Build("Free_Cam");

	pObjMgr->Add_Object(Camera, { "Test_Level","Camera_Layer" });
	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
}

void CTestLevel::Rake_MapResources()
{
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	for (const auto& entry : filesystem::recursive_directory_iterator("../Bin/Resources/MapData/Model/"))
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

HRESULT CTestLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CTestLevel::PreLoad_Level()
{
}

CTestLevel* CTestLevel::Create(const string& LevelKey)
{
	CTestLevel* instance = new CTestLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CTestLevel::Free()
{
	m_pGameInstance->DestroyInstance();
}
