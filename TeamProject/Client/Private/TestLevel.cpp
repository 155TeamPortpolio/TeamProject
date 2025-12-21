#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCam.h"
#include "TestObject.h"
#include "TestFloor.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "SequenceCam.h"
#include "RigidBody.h"
#include "CharacterController.h"

#include "Camera.h"

/* MapData */
#include "MapLoader.h"
#include "MapPlacedObject.h"

CTestLevel::CTestLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
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

	// ============ Camera ==================================================
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_OrbitCam",    COrbitCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_FreeCam",     CFreeCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SequenceCam", CSequenceCam::Create());
	// =========================================================================

	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestFloor", CTestFloor::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());

	//// Ready MapObject key and path to ResourceMgr 
	//Rake_MapResources();

	////Map Loader Logic is going to Change
	//CMapLoader* pMapLoader = CMapLoader::Create("Test_Level",  "../Bin/Resources/MapData/Data/MapTool.Data_1_20251217_200942.json" );
	//if (nullptr == pMapLoader)
	//	MSG_BOX("Failed to Load MapData!");
	//Safe_Release(pMapLoader);

	auto objMgr = m_pGameInstance->Get_ObjectMgr();
	auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
		.CharacterController({})
		.Build("Test_Model");

	objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer"});


	COLLIDER_DESC colDesc;
	colDesc.bCooking = true;
	colDesc.strModelKey = "Concert_Ground_FloorTile_01.model";
	CGameObject* pTestFloor = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestFloor" })
		.Collider(colDesc)
		.Build("Test_Floor");

	objMgr->Add_Object(pTestFloor, { "Test_Level", "Model_Layer" });
	// --------------------------- Camera -------------------------------------------------
	constexpr float kAspect = (float)g_iWinSizeX / g_iWinSizeY;

	auto orbitCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OrbitCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("Orbit_Cam");
	static_cast<COrbitCam*>(orbitCam)->SetTarget(testModel);

	auto sequenceCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_SequenceCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("SequenceCam");

	auto freeCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_FreeCam"})
		.Camera(kAspect)
		.Position({0.f, 2.f, -3.f})
		.Build("FreeCam");

	objMgr->Add_Object(orbitCam,    {"Test_Level", "Camera_Layer" });
	objMgr->Add_Object(sequenceCam, {"Test_Level", "Camera_Layer" });
	objMgr->Add_Object(freeCam,     {"Test_Level", "Camera_Layer" });

	m_pCamDirector->Bind(static_cast<CSequenceCam*>(sequenceCam));
	m_pCamDirector->Register("Intro", "../bin/Resources/Intro_2.cam");

	//CAM->Set_MainCam(orbitCam->Get_Component<CCamera>());
	CAM->Set_MainCam(freeCam->Get_Component<CCamera>());

	Ready_Camera();
	return S_OK;
}

void CTestLevel::Update()
{
	auto dt = m_pGameInstance->Get_EngineDeltaTime();

	m_pCamDirector->Update(dt);

	auto input = m_pGameInstance->Get_InputDev();

	if (input->Key_Down('C'))
		m_sequenceHandle = m_pCamDirector->RequestSequence("Intro", 1.f, true, 1.f);
	
	if (input->Key_Down('1'))
		CAM->SetShake(1.5f, 12.f, 0.2f, 0.12f);
	if (input->Key_Down('2'))
		CAM->AddShake(3.0f, 8.f, 0.35f, 0.2f);
	if (input->Key_Down('3'))
		CAM->ClearShake();
}

void CTestLevel::Ready_Camera()
{

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
	__super::Free();
	m_pCamDirector->DestroyInstance();
	m_pGameInstance->DestroyInstance();
}
