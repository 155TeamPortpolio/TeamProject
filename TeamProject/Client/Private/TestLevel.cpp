#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"
#include "Helper_Func.h"

#include "TestMap.h"
#include "TestObject.h"
#include "TestFloor.h"
#include "RigidBody.h"
#include "CharacterController.h"

#include "BattleSystem.h"
#include "FieldSystem.h"
#include "DataBase.h"
#include "MonsterSpawnConsole.h"

// Camera
#include "Camera.h"
#include "FreeCam.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "ShadowCam.h"
#include "SequenceCam.h"
#include "CamPanel.h"
#include "CamLoader.h"

/* MapData */
#include "MapLoader.h"
#include "MapPlacedObject.h"
#include "MapTriggerObject.h"

/* Effect */
#include "MeshNode.h"
#include "SpriteNode.h"
#include "ParticleNode.h"
#include "TrailNode.h"
#include "EffectContainer.h"
#include "AttackSign.h"

/* Character */
#include "Miyabi.h"
#include "Anbi.h"
#include "Corin.h"
#include "JaneDoe.h"
#include "Player.h"
#include "BattlePlayer.h"

/* Enemy */
#include "EnemyAttackCollider.h"
#include "EnemyTriggerCollider.h"
#include "ThugBulkyEnforcer.h"
#include "ThugAssaulter.h"
#include "Defiler.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Arrow.h"
#include "Claymore.h"

/*npc*/
#include "OfficeMeow.h"
#include "BangBooPay.h"
#include "BangBooAsk.h"
#include "BangBooDeliver.h"
#include "Howl.h"
#include "ElectricBoo.h"

/* UI */
#include "UIDirector.h"

/* Interactable */
#include "Portal.h"

/* ShaderTest */
#include "TestCloud.h"

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
	//if (FAILED(CBattleSystem::GetInstance()->LoadMonsterCreationTable("../../Resources/Data/MonsterTable/MonsterTable.csv")))
	//	MSG_BOX("Failed to Load MonsterTable!");


	RenderSystem()->Set_FogDesc({ _float4(0.12f, 0.25f, 0.35f, 1.0f),0.f, 0.f, 0.005f, true });

	return S_OK;
}

HRESULT CTestLevel::Awake()
{
	m_pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
	m_pPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);

	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(true);

	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	IResourceService* pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	auto objMgr = m_pGameInstance->Get_ObjectMgr();

	//============== Test =================================
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestPlane", CTestPlane::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestFloor", CTestFloor::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestMap", CTestMap::Create());

	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Test_Level");

	//==================== Interactable ===============
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Portal", CPortal::Create());
	
	//============== Map ============================
	//Ready_Map("Test_Level", "Zero_Worksite");
	Ready_Map("Test_Level", "TrainingRoom");

	/* Miyabi */
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Miyabi", CCorin::Create());
	//CCT_DESC miyabiCCT;
	//miyabiCCT.eGroup = COLLISION_GROUP::PLAYER;
	//miyabiCCT.iCollisionMask = 0xFFFFFFFF;
	////miyabiCCT.iCollisionMask = 0xFFFFFFFF & ~ENUM(COLLISION_GROUP::COMMON);
	//miyabiCCT.bAutoFit = false;
	//miyabiCCT.fHeight = 1.28f;
	//miyabiCCT.fRadius = 0.2f;
	//miyabiCCT.eGroup = COLLISION_GROUP::PLAYER;
	////miyabiCCT.fBoundingMinY = -0.88f;
	//miyabiCCT.vPos = { 0.f, 1.5f, 0.f };
	//auto Miyabi = Builder::Create_Object({ "Test_Level", "Proto_GameObject_Miyabi" })
	//	.Position(_float3(3.f, 0.f, 0.f))
	//	.CharacterController(miyabiCCT)
	//	.Build("Miyabi");
	//objMgr->Add_Object(Miyabi, { "Test_Level", "Model_Layer" });
	//
	//m_miyabiHandle = Miyabi->Get_Handle();


	/* Enemy */
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ThugBulkyEnforcer", CThugBulkyEnforcer::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_EnemyTriggerCollider", CEnemyTriggerCollider::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ThugAssaulter", CThugAssaulter::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Defiler", CDefiler::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ThugPoacher", CThugPoacher::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ThugPoacher_Arrow", CThugPoacher_Arrow::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Claymore", CClaymore::Create());

	CBattleSystem::GetInstance()->ReadyBattle("TrainingRoom", 1);
	// It will be changed soooooon
	CBattleSystem::GetInstance()->SetActive(true);

	//====================Test=================
	//Ready_TestObject();
	//Ready_Npc();

	CamDirector()->StartBattleIntro(CamSeqType::BattleIntro);
	//CUIDirector::GetInstance()->Show_SceneFrame();
	CUIDirector::GetInstance()->Show_HUD(CUIDirector::HUD::BATTLE);
	//GameInstance()->Set_EngineTimeScale(0.05f);

#ifdef  _USING_GUI
	Ready_MonsterSpawnConsole();
#endif

	return S_OK;
}

void CTestLevel::Update()
{
	CBattleSystem::GetInstance()->Update();

	if (InputDevice()->Key_Tap(VK_F4))
	{
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_Sacrifice", { 0.f, 0.5f,0.f });
	}
	if (InputDevice()->Key_Tap(VK_F5))
	{
		auto rushTrail = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_rush_trail.json")
			.Build("Rush");
		
		ObjectManager()->Add_Object(rushTrail, { "Test_Level","Effect_Layer" });
	}
	
	// [`] 
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_OEM_3)) {
	//	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -0.18f, 0.f,1.59f });
	//}	
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_F6)) {
	//	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -0.18f, 0.f,5.f });
	//}
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_F7)) {
	//	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugPoacher", { -0.18f, 0.f, 7.f });
	//}
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_F8)) {
	//	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_Defiler", { -0.18f, 0.f,5.f });
	//}
}

void CTestLevel::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	//// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();
	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
}

void CTestLevel::Rake_MapResources()
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

void CTestLevel::Ready_TestObject()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	auto objMgr = m_pGameInstance->Get_ObjectMgr();

	//==============TestEffect==========================
	//pResource->Add_ResourcePath("glow_particle.json", "../Bin/Resources/Effect/glow_particle.json");
	//pResource->Add_ResourcePath("Eff_Disorder_UU_23.png", "../Bin/Resources/Effect/Eff_Disorder_UU_23.png");

	//EFFECT_ASSET EffectAsset = pResource->Load_EffectAsset(G_GlobalLevelKey, "glow_particle.json");
	//auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
	//	.Asset("glow_particle.json")
	//	.Position(_float3(0.f, 0.f, 0.f))
	//	.Build("Test_Effect");
	//objMgr->Add_Object(effect, { "Test_Level","Effect_Layer" });

	//==============TestModel==========================
	//auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
	//	.CharacterController({})
	//	.Build("Test_Model");
	
	//objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer"});
	
	// =================TestMap==================
	//auto testMap = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestMap" })
	//	.Build("Test_Map");
	//
	//objMgr->Add_Object(testMap, { "Test_Level", "Model_Layer" });


	// =====================TestFloor=========================
	//COLLIDER_DESC colDesc;
	//colDesc.bCooking = true;
	//colDesc.strModelKey = "Concert_Ground_FloorTile_01.model";
	//
	//for (_int z = 0; z < 3; ++z)
	//{
	//	for (_int x = 0; x < 3; ++x)
	//	{
	//		CGameObject* pTestFloor = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestFloor" })
	//			.Collider(colDesc)
	//			.Position({ x * 6.15f, 0.f, z * 6.15f })
	//			.Build("Test_Floor_" + to_string(z * 3 + x));
	//		objMgr->Add_Object(pTestFloor, { "Test_Level", "Model_Layer" });
	//	}
	//}

	// =====================TestCloud=========================
}

void CTestLevel::Ready_Npc()
{
	auto pProto = PrototypeManager();
	auto objMgr = ObjectManager();

	/*Npc*/
	CCT_DESC meowCCT;
	//meowCCT.eGroup = COLLISION_GROUP::PLAYER;
	meowCCT.iCollisionMask = 0xFFFFFFFF;
	//miyabiCCT.iCollisionMask = 0xFFFFFFFF & ~ENUM(COLLISION_GROUP::COMMON);
	meowCCT.bAutoFit = false;
	meowCCT.fHeight = 1.6f;
	meowCCT.fRadius = 0.4f;
	meowCCT.eGroup = COLLISION_GROUP::COMMON;
	//meowCCT.fBoundingMinY = -0.83f;
	meowCCT.vPos = { 25.f, 1.5f, -30.f };
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_OfficeMeow", COfficeMeow::Create());
	auto testMeow = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OfficeMeow" })
		.CharacterController(meowCCT)
		.Build("Test_Meow");

	objMgr->Add_Object(testMeow, { "Test_Level", "Npc_Layer" });

	CCT_DESC bangbooCCT;
	//meowCCT.eGroup = COLLISION_GROUP::PLAYER;
	bangbooCCT.iCollisionMask = 0xFFFFFFFF;
	//miyabiCCT.iCollisionMask = 0xFFFFFFFF & ~ENUM(COLLISION_GROUP::COMMON);
	bangbooCCT.bAutoFit = false;
	bangbooCCT.fHeight = 0.6f;
	bangbooCCT.fRadius = 0.4f;
	bangbooCCT.eGroup = COLLISION_GROUP::COMMON;
	//meowCCT.fBoundingMinY = -0.83f;
	bangbooCCT.vPos = { 22.5f, 1.5f, -30.f };
	
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_BangBooPay", CBangBooPay::Create());
	auto testBoo = Builder::Create_Object({ "Test_Level", "Proto_GameObject_BangBooPay" })
		.CharacterController(bangbooCCT)
		.Build("Test_Pay");

	objMgr->Add_Object(testBoo, { "Test_Level", "Npc_Layer" });

	bangbooCCT.vPos = { 21.7f, 1.5f, -30.f };
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_BangBooAsk", CBangBooAsk::Create());
	testBoo = Builder::Create_Object({ "Test_Level", "Proto_GameObject_BangBooAsk" })
		.CharacterController(bangbooCCT)
		.Build("Test_Ask");

	objMgr->Add_Object(testBoo, { "Test_Level", "Npc_Layer" });

	bangbooCCT.vPos = { 20.9f, 1.5f, -30.f };
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_BangBooDeliver", CBangBooDeliver::Create());
	testBoo = Builder::Create_Object({ "Test_Level", "Proto_GameObject_BangBooDeliver" })
		.CharacterController(bangbooCCT)
		.Build("Test_Deliver");

	objMgr->Add_Object(testBoo, { "Test_Level", "Npc_Layer" });

	CCT_DESC howlCCT;
	//meowCCT.eGroup = COLLISION_GROUP::PLAYER;
	howlCCT.iCollisionMask = 0xFFFFFFFF;
	//miyabiCCT.iCollisionMask = 0xFFFFFFFF & ~ENUM(COLLISION_GROUP::COMMON);
	howlCCT.bAutoFit = false;
	howlCCT.fHeight = 0.7f;
	howlCCT.fRadius = 0.4f;
	howlCCT.eGroup = COLLISION_GROUP::COMMON;
	//meowCCT.fBoundingMinY = -0.83f;
	howlCCT.vPos = { 19.f, 1.5f, -30.f };
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Howl", CElectricBoo::Create());
	auto testHowl = Builder::Create_Object({ "Test_Level", "Proto_GameObject_Howl" })
		.CharacterController(howlCCT)
		.Build("Test_Howl");

	objMgr->Add_Object(testHowl, { "Test_Level", "Npc_Layer" });
}

#ifdef  _USING_GUI
HRESULT CTestLevel::Ready_MonsterSpawnConsole()
{
	auto pGuiSystem = m_pGameInstance->Get_GUISystem();

	CMonsterSpawnConsole* pConsole = CMonsterSpawnConsole::Create(pGuiSystem->Get_Context());
	if (nullptr == pConsole)
		return E_FAIL;

	pGuiSystem->Register_Panel(pConsole); 
	
	return S_OK;
}
#endif

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
	//CBattleSystem::GetInstance()->DestroyInstance();
	//CDataBase::GetInstance()->DestroyInstance();
	m_pCamDirector->DestroyInstance();
	m_pGameInstance->DestroyInstance();
	m_pPlayer->Clear_Characters();
}
