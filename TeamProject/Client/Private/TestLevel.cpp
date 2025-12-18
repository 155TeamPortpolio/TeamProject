#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCam.h"
#include "TestObject.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "SequenceCam.h"

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

	// ============ Camera ==================================================
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_OrbitCam",    COrbitCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_FreeCam",     CFreeCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SequenceCam", CSequenceCam::Create());
	// =========================================================================

	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());

	// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();

	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create("Test_Level",  "../Bin/Resources/MapData/Data/MapTool.Data_1_20251217_200942.json" );
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);

	auto objMgr = m_pGameInstance->Get_ObjectMgr();
	auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
		.Build("Test_Model");
	testModel->Get_Component<CTransform>()->Set_Pos({0.f, 5.f, 0.f});
	objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer"});

	m_testModel = testModel;
	Safe_AddRef(m_testModel);

	Ready_Camera();
	return S_OK;
}

void CTestLevel::Update()
{
	auto dt = m_pGameInstance->Get_EngineDeltaTime();

	m_pCamDirector->Update(dt);

	auto input = m_pGameInstance->Get_InputDev();

	if (input->Key_Down('C'))
		m_sequenceHandle = m_pCamDirector->RequestSequence("Intro", 1.f, true);

}

void CTestLevel::Ready_Camera()
{
	assert(m_testModel);

	constexpr float kAspect = (float)g_iWinSizeX / g_iWinSizeY;
	auto objMgr = m_pGameInstance->Get_ObjectMgr();

	auto sequenceCamObj = Builder::Create_Object({ "Test_Level", "Proto_GameObject_SequenceCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("SequenceCam");
	objMgr->Add_Object(sequenceCamObj, { "Test_Level", "Camera_Layer" });

	auto orbitCamObj = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OrbitCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("Orbit_Cam");
	objMgr->Add_Object(orbitCamObj, { "Test_Level", "Camera_Layer" });

	m_orbitCam = static_cast<COrbitCam*>(orbitCamObj);
	Safe_AddRef(m_orbitCam);

	CTransform* targetTr = m_testModel->Get_Component<CTransform>();
	assert(targetTr);
	m_orbitCam->SetTarget(targetTr);

	CAM->Set_MainCam(orbitCamObj->Get_Component<CCamera>());

	if (!m_pCamDirector)
		m_pCamDirector = CCamDirector::Create();

	m_pCamDirector->Bind(static_cast<CSequenceCam*>(sequenceCamObj));
	m_pCamDirector->Register("Intro", "../bin/Resources/Intro_2.cam");
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
	Safe_Release(m_pCamDirector);
	Safe_Release(m_testModel);
	Safe_Release(m_orbitCam);
	m_pGameInstance->DestroyInstance();
}
