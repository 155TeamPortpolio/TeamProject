#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCam.h"
#include "TestObject.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "SequenceCam.h"

#include "Camera.h"

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

	auto objMgr = m_pGameInstance->Get_ObjectMgr();
	auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
		.Build("Test_Model");
	objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer"});

	Ready_Camera();
	return S_OK;
}

void CTestLevel::Update()
{
	if (m_pCamDirector)
		m_pCamDirector->Update(m_pGameInstance->Get_EngineDeltaTime());

	auto input = m_pGameInstance->Get_InputDev();

	if (input->Key_Down('2'))
		m_sequenceHandle = m_pCamDirector->RequestSequence("Intro", 0.25f, true);

	if (input->Key_Down('1'))
		m_pCamDirector->StopAll(0.25f);
}

void CTestLevel::Ready_Camera()
{
	constexpr float kAspect = (float)g_iWinSizeX / g_iWinSizeY;
	auto objMgr = m_pGameInstance->Get_ObjectMgr();
	 // ---------------- FreeCam ---------------------------------------------------
	auto freeCam = Builder::Create_Object({ "Test_Level" ,"Proto_GameObject_FreeCam" })
		.Camera(kAspect)
		.Position({ 0.f, 3.f, -3.f })
		.Build("FreeCam");

	objMgr->Add_Object(freeCam, { "Test_Level","Camera_Layer" });
//	CAM->Set_MainCam(freeCam->Get_Component<CCamera>());
	// ----------------- SequenceCam -------------------------------------------------
	auto sequenceCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_SequenceCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("SequenceCam");

	objMgr->Add_Object(sequenceCam, { "Test_Level", "Camera_Layer" });
	// ----------------- OrbitCam ---------------------------------------------------
	auto orbitCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OrbitCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("Orbit_Cam");

	objMgr->Add_Object(orbitCam, { "Test_Level", "Camera_Layer" });

	CAM->Set_MainCam(orbitCam->Get_Component<CCamera>());

	if (!m_pCamDirector)
		m_pCamDirector = CCamDirector::Create();

	m_pCamDirector->Bind(static_cast<CSequenceCam*>(sequenceCam));
	m_pCamDirector->Register("Intro", "../bin/Resources/Intro.cam");
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
}
