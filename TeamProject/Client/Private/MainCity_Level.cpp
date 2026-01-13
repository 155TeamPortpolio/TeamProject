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
#include "MapDataCloud.h"
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
	m_pMapDataCloud = CMapDataCloud::Create("../Bin/Resources/MapData/Data/");
	if (nullptr == m_pMapDataCloud)
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCity_Level::Awake()
{
	//============== Map ============================
	ReadyMap();
	CUIDirector::GetInstance()->Load_LevelObjects("MainCity_Level");
	ReadyCamera();
	ReadyShadowCamera();
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
	// ============ Camera ==================================================
	PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_OrbitCam", COrbitCam::Create());
	PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_ShadowCam", CShadowCam::Create());

	//============== Map ============================
	PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());
	PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_MapTriggerObject", CMapTriggerObject::Create());
}

void CMainCity_Level::ReadyMap()
{
	CMapLoader* pMapLoader = CMapLoader::Create(m_LevelKey, m_pMapDataCloud, "TrainingRoom");
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
}

void CMainCity_Level::ReadyCamera()
{
	_float2 clientSize = GameInstance()->Get_ClientSize();
	_float aspect =clientSize.x / clientSize.y;

	CCT_DESC desc;
	desc.eGroup = COLLISION_GROUP::CAMERA;

	auto orbitCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OrbitCam" })
		.Camera(aspect)
		.CharacterController(desc)
		.Build("OrbitCam");

	ObjectManager()->Add_Object(orbitCam, { "Test_Level", "Camera_Layer" });
	m_pCamDirector->SetCam(CamType::Orbit, orbitCam->Get_Handle());
	m_pCamDirector->SetReturnCam(CamType::Orbit);
	CameraManager()->Set_MainCam(orbitCam->Get_Component<CCamera>());
}

void CMainCity_Level::ReadyShadowCamera()
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
