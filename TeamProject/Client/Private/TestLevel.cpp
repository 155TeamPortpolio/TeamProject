#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCam.h"
#include "TestObject.h"
#include "TestPlane.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "SequenceCam.h"
#include "RigidBody.h"
#include "CharacterController.h"

#include "Camera.h"

/* MapData */
#include "MapLoader.h"
#include "MapPlacedObject.h"

/* Effect */
#include "EffectContainer.h"
#include "ParticleNode.h"
#include "SpriteNode.h"
#include "MeshNode.h"

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
	auto objMgr = m_pGameInstance->Get_ObjectMgr();
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	IResourceService* pResource = CGameInstance::GetInstance()->Get_ResourceMgr();

	// ============ Camera ==================================================
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_OrbitCam",    COrbitCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_FreeCam",     CFreeCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SequenceCam", CSequenceCam::Create());
	// =========================================================================

	//==================== Effect =======================
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SpriteNode", CSpriteNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ParticleNode", CParticleNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MeshNode", CMeshNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EffectContainer", CEffectContainer::Create());
	
	pResource->Add_ResourcePath("test_particle.json", "../Bin/Resources/Effect/test_particle.json");
	pResource->Add_ResourcePath("Eff_Particle_044.png", "../Bin/Resources/Effect/Eff_Particle_044.png");
	
	//EFFECT_ASSET EffectAsset = pResource->Load_EffectAsset(G_GlobalLevelKey, "test_particle.json");
	auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("test_particle.json")
		.Position(_float3(0.f, 0.f, 0.f))
		.Build("Test_Effect");
	objMgr->Add_Object(effect, { "Test_Level","Effect_Layer" });
	//===================================================

	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestPlane", CTestPlane::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());

	// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();

	//Map Loader Logic is going to Change
	//CMapLoader* pMapLoader = CMapLoader::Create("Test_Level",  "../Bin/Resources/MapData/Data/MapTool.Data_1_20251217_200942.json" );
	//if (nullptr == pMapLoader)
	//	MSG_BOX("Failed to Load MapData!");
//	Safe_Release(pMapLoader);

	auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
		.CharacterController({})
		.Build("Test_Model");

	COLLIDER_DESC desc;
	desc.bCooking = true;
	desc.strModelKey = "Concert_Ground_FloorTile_01.model";

	auto testPlane = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestPlane" })
		.Collider(desc)
		.Build("Test_Plane");
	//testModel->Get_Component<CTransform>()->Set_Pos({0.f, 10.f, 0.f});
	auto cc = testModel->Get_Component<CCharacterController>();

	objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer" });
	objMgr->Add_Object(testPlane, { "Test_Level", "Model_Layer" });


	// --------------------------- Camera -------------------------------------------------
	constexpr float kAspect = (float)g_iWinSizeX / g_iWinSizeY;

	auto orbitCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OrbitCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("Orbit_Cam");
	static_cast<COrbitCam*>(orbitCam)->SetTarget(testModel);

	objMgr->Add_Object(orbitCam, { "Test_Level", "Camera_Layer" });

	auto sequenceCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_SequenceCam" })
		.Camera(kAspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("SequenceCam");
	objMgr->Add_Object(sequenceCam, { "Test_Level", "Camera_Layer" });

	m_pCamDirector->Bind(static_cast<CSequenceCam*>(sequenceCam));
	//m_pCamDirector->Register("Intro", "../bin/Resources/Intro_2.cam");
	m_pCamDirector->Register("Intro", "../bin/Resources/Test.cam");
	m_pCamDirector->SetSpaceReference(testModel->Get_Handle());

	CAM->Set_MainCam(orbitCam->Get_Component<CCamera>());

	Ready_Camera();
	return S_OK;
}

void CTestLevel::Update()
{
	auto dt = m_pGameInstance->Get_EngineDeltaTime();

	m_pCamDirector->Update(dt);

	auto input = m_pGameInstance->Get_InputDev();

	if (input->Key_Down('C'))
		m_sequenceHandle = m_pCamDirector->RequestSequence("Intro", 2.f, true, 2.f);
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
