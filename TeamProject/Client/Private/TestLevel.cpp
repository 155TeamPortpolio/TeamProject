#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "TestMap.h"
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
#include "MapDataCloud.h"
#include "MapLoader.h"
#include "MapPlacedObject.h"

/* Effect */
#include "MeshNode.h"
#include "SpriteNode.h"
#include "ParticleNode.h"
#include "EffectContainer.h"

CTestLevel::CTestLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pCamDirector);
}

HRESULT CTestLevel::Initialize()
{
	m_pMapDataCloud = CMapDataCloud::Create("../Bin/Resources/MapData/Data/");
	if (nullptr == m_pMapDataCloud)
		return E_FAIL;


	return S_OK;
}

HRESULT CTestLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	IResourceService* pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	auto objMgr = m_pGameInstance->Get_ObjectMgr();

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
	//pResource->Add_ResourcePath("glow_particle.json", "../Bin/Resources/Effect/glow_particle.json");
	//pResource->Add_ResourcePath("Eff_Disorder_UU_23.png", "../Bin/Resources/Effect/Eff_Disorder_UU_23.png");
	
	//EFFECT_ASSET EffectAsset = pResource->Load_EffectAsset(G_GlobalLevelKey, "glow_particle.json");
	//auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
	//	.Asset("glow_particle.json")
	//	.Position(_float3(0.f, 0.f, 0.f))
	//	.Build("Test_Effect");
	//objMgr->Add_Object(effect, { "Test_Level","Effect_Layer" });
	//===================================================

	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestPlane", CTestPlane::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestFloor", CTestFloor::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestMap", CTestMap::Create());

	//// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();

	//Map Loader Logic is going to Change
	//CMapLoader* pMapLoader = CMapLoader::Create("Test_Level", m_pMapDataCloud, "Test");
	//if (nullptr == pMapLoader)
	//	MSG_BOX("Failed to Load MapData!");
	//Safe_Release(pMapLoader);

	auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
		.CharacterController({})
		.Build("Test_Model");

	objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer"});

	auto testMap = Builder::Create_Object({"Test_Level", "Proto_GameObject_TestMap"})
		.Build("Test_Map");

	objMgr->Add_Object(testMap, {"Test_Level", "Model_Layer"});

	COLLIDER_DESC colDesc;
	colDesc.bCooking = true;
	colDesc.strModelKey = "Concert_Ground_FloorTile_01.model";

	for (_int z = 0; z < 3; ++z)
	{
	for (_int x = 0; x < 3; ++x)
		{
			CGameObject* pTestFloor = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestFloor" })
				.Collider(colDesc)
				.Position({ x * 6.5f, 0.f, z * 6.5f })
				.Build("Test_Floor_" + to_string(z * 3 + x));
			objMgr->Add_Object(pTestFloor, { "Test_Level", "Model_Layer" });
		}
	}
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

	Safe_Release(m_pMapDataCloud);
	m_pCamDirector->DestroyInstance();
	m_pGameInstance->DestroyInstance();
}
