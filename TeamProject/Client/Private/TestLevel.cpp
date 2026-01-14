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
#include "DataBase.h"

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

/* Enemy */
#include "Sacrifice.h" 
#include "SacrificeHand.h"
#include "Sacrifice_Laser.h"
#include "Sacrifice_Orb.h"
#include "ThugBulkyEnforcer.h"
#include "EnemyAttackCollider.h"
#include "EnemyTriggerCollider.h"
#include "ThugAssaulter.h"

/*npc*/
#include "OfficeMeow.h"
#include "BangBooPay.h"
#include "BangBooAsk.h"
#include "BangBooDeliver.h"

/* UI */
#include "UIDirector.h"

/* ShaderTest */
#include "TestCloud.h"

CTestLevel::CTestLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTestLevel::Initialize()
{
	//if (FAILED(CBattleSystem::GetInstance()->LoadMonsterCreationTable("../../Resources/Data/MonsterTable/MonsterTable.csv")))
	//	MSG_BOX("Failed to Load MonsterTable!");


	// It will be changed soooooon
	CBattleSystem::GetInstance()->SetActive(true);

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
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ShadowCam", CShadowCam::Create());
	// =========================================================================

	//==================== Effect =======================
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SpriteNode", CSpriteNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ParticleNode", CParticleNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MeshNode", CMeshNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_TrailNode", CTrailNode::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EffectContainer", CEffectContainer::Create());
	pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_AttackSign", CAttackSign::Create());

	/* Assets */
	pResource->Add_ResourcePath("test_particle.json", "../Bin/Resources/Effect/Data/test_particle.json");
	pResource->Add_ResourcePath("spawn_smoke.json", "../Bin/Resources/Effect/Data/spawn_smoke.json");
	pResource->Add_ResourcePath("fog.json", "../Bin/Resources/Effect/Data/fog.json");
	pResource->Add_ResourcePath("hit_ground_smoke.json", "../Bin/Resources/Effect/Data/hit_ground_smoke.json");
	pResource->Add_ResourcePath("hit_ground_smoke_strong.json", "../Bin/Resources/Effect/Data/hit_ground_smoke_strong.json");
	pResource->Add_ResourcePath("core.json", "../Bin/Resources/Effect/Data/core.json");
	pResource->Add_ResourcePath("rock_particle.json", "../Bin/Resources/Effect/Data/rock_particle.json");
	pResource->Add_ResourcePath("sacrifice_spark.json", "../Bin/Resources/Effect/Data/sacrifice_spark.json");
	pResource->Add_ResourcePath("sacrifice_hit_ground_flare.json", "../Bin/Resources/Effect/Data/sacrifice_hit_ground_flare.json");
	pResource->Add_ResourcePath("sacrifice_hit_ground_flare_smoke.json", "../Bin/Resources/Effect/Data/sacrifice_hit_ground_flare_smoke.json");
	pResource->Add_ResourcePath("sacrifice_smoke_trail.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail.json");
	pResource->Add_ResourcePath("sacrifice_smoke_trail.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail.json");
	pResource->Add_ResourcePath("sacrifice_smoke_trail_cone.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail_cone.json");
	pResource->Add_ResourcePath("sacrifice_orb.json", "../Bin/Resources/Effect/Data/sacrifice_orb.json");
	pResource->Add_ResourcePath("sacrifice_smoke_slash.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_slash.json");

	/* Textures */
	pResource->Add_ResourcePath("attack_sign.png", "../Bin/Resources/Effect/Texture/attack_sign.png");
	pResource->Add_ResourcePath("Eff_Particle_044.png", "../Bin/Resources/Effect/Texture/Eff_Particle_044.png");
	pResource->Add_ResourcePath("Eff_Smoke_046_LB_01.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_046_LB_01.png");
	pResource->Add_ResourcePath("Eff_Smoke_218.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_218.png");
	pResource->Add_ResourcePath("Eff_Smoke_006.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_006.png");
	pResource->Add_ResourcePath("rock0.png", "../Bin/Resources/Effect/Texture/rock0.png");
	pResource->Add_ResourcePath("lightning10.png", "../Bin/Resources/Effect/Texture/lightning10.png");
	pResource->Add_ResourcePath("lightning7.png", "../Bin/Resources/Effect/Texture/lightning7.png");
	pResource->Add_ResourcePath("Flare_UU_02.png", "../Bin/Resources/Effect/Texture/Flare_UU_02.png");
	pResource->Add_ResourcePath("Eff_Burn_LYX_28.png", "../Bin/Resources/Effect/Texture/Eff_Burn_LYX_28.png");
	pResource->Add_ResourcePath("Eff_Smoke_259.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_259.png");

	/* Models */
	pResource->Add_ResourcePath("Smoke_Cone2.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Trail/Smoke_Cone2.model");
	pResource->Add_ResourcePath("Smoke_Cone2.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Trail/Smoke_Cone2.mat");
	pResource->Add_ResourcePath("Sacrifice_Orb.model", "../Bin/Resources/Effect/Model/Sacrifice_Orb/Sacrifice_Orb.model");
	pResource->Add_ResourcePath("Sacrifice_Orb.mat", "../Bin/Resources/Effect/Model/Sacrifice_Orb/Sacrifice_Orb.mat");
	pResource->Add_ResourcePath("Sacrifice_Smoke_Slash5.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash5/Sacrifice_Smoke_Slash5.model");
	pResource->Add_ResourcePath("Sacrifice_Smoke_Slash5.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash5/Sacrifice_Smoke_Slash5.mat");
	pResource->Add_ResourcePath("Sacrifice_Smoke_Slash6.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash6/Sacrifice_Smoke_Slash6.model");
	pResource->Add_ResourcePath("Sacrifice_Smoke_Slash6.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash6/Sacrifice_Smoke_Slash6.mat");
	
	//============== Test =================================
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestPlane", CTestPlane::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestFloor", CTestFloor::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestMap", CTestMap::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestPlayer", CPlayer::Create());
	auto Player = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestPlayer" })
		.Build("Test_Player");

	objMgr->Add_Object(Player, { "Test_Level", "Model_Layer" });
	//============== Map ============================
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_MapTriggerObject", CMapTriggerObject::Create());

	//============== Map ============================
	Ready_Map("Test_Level", "Zero_Worksite");

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
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Sacrifice", CSacrifice::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SacrificeHand", CSacrificeHand::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SacrificeLaser", CSacrifice_Laser::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SacrificeOrb", CSacrifice_Orb::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ThugBulkyEnforcer", CThugBulkyEnforcer::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_EnemyTriggerCollider", CEnemyTriggerCollider::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_ThugAssaulter", CThugAssaulter::Create());

	// --------------------------- Camera -------------------------------------------------
	Ready_Camera();

	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Test_Level");

	//====================Test=================
	Ready_TestObject();
	Ready_ShadowCamera();
	Ready_Npc();

	m_pCamDirector->SetSpaceRef(CBattleSystem::GetInstance()->GetCurCharacterHandle());
	m_pCamDirector->RequestSequence("Jane_Intro", 0.f, true, 0.5f);

	return S_OK;
}

void CTestLevel::Update()
{
	CBattleSystem::GetInstance()->Update();

	static OBJECT_HANDLE prevPlayer{};

	OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();

	if (curPlayer.isValid() && curPlayer.Get() != prevPlayer.Get())
	{
		prevPlayer = curPlayer;

		m_pCamDirector->SetSpaceRef(curPlayer);

		auto orbitObj = ObjectManager()->Request_Object(m_pCamDirector->GetCamHandle(CamType::Orbit));
		static_cast<COrbitCam*>(orbitObj)->SetTarget(curPlayer);
	}

	if (InputDevice()->Key_Down(VK_F1))
	{
		auto obj = ObjectManager()->Request_Object(m_pCamDirector->GetCamHandle(CamType::Free));
		CameraManager()->Set_MainCam(obj->Get_Component<CCamera>(), 0.5f);
	}

	if (InputDevice()->Key_Down(VK_F2))
	{
		const OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();

		auto obj = ObjectManager()->Request_Object(m_pCamDirector->GetCamHandle(CamType::Orbit));
		static_cast<COrbitCam*>(obj)->SetTarget(curPlayer);

		CameraManager()->Set_MainCam(obj->Get_Component<CCamera>(), 0.5f);
	}

	if (InputDevice()->Key_Down(VK_F3))
		m_pCamDirector->RequestSequence("Jane_Intro", 0.f, true, 0.5f);

	if (InputDevice()->Key_Tap(VK_F7))
		CameraManager()->AddShake(CamShakeType::HitNormal);

	if (InputDevice()->Key_Tap(VK_F8))
		CameraManager()->AddShake(CamShakeType::HitHeavy);

	//	m_pCamDirector->RequestSequence("Jane_Intro_2", 0.f, true, 0.5f);

	m_pCamDirector->Update(m_pGameInstance->Get_EngineDeltaTime());

	if (InputDevice()->Key_Tap(VK_F4))
	{
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_Sacrifice", { 0.f, 0.5f,0.f });
	}

	if(InputDevice()->Key_Tap(VK_F5))
	{
		auto pOrb = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_smoke_slash.json")
			.Build("Orb");

		auto pTransform = pOrb->Get_Component<CTransform>();
		_quaternion q(0.07f, 0.06f, 0.99f, 0.12f);
		q.Normalize();
		pTransform->Set_Quaternion(q);
		ObjectManager()->Add_Object(pOrb, { "Test_Level","Effect_Layer" });
	}

	// [`] 
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_OEM_3)) {
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -0.18f, 0.f,1.59f });
	}	
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_F6)) {
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -0.18f, 0.f,5.f });
	}
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

void CTestLevel::Ready_Camera()
{
	constexpr _float aspect = (_float)g_iWinSizeX / g_iWinSizeY;

	auto seqCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_SequenceCam"})
		.Camera(aspect)
		.Position({0.f, 2.f, -5.f})
		.Build("SequenceCam");

	auto freeCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_FreeCam"})
		.Camera(aspect)
		.Position({0.f, 2.f, -3.f})
		.Build("FreeCam");

	CCT_DESC desc;
	desc.eGroup         = COLLISION_GROUP::CAMERA;
	desc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);

	auto orbitCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_OrbitCam"})
		.Camera(aspect)
		.CharacterController(desc)
		.Build("OrbitCam");

	ObjectManager()->Add_Object(seqCam,   {"Test_Level", "Camera_Layer"});
	ObjectManager()->Add_Object(freeCam,  {"Test_Level", "Camera_Layer"});
	ObjectManager()->Add_Object(orbitCam, {"Test_Level", "Camera_Layer"});

	m_pCamDirector->SetCam(CamType::Sequence, seqCam->Get_Handle());
	m_pCamDirector->SetCam(CamType::Free,     freeCam->Get_Handle());
	m_pCamDirector->SetCam(CamType::Orbit,    orbitCam->Get_Handle());

	m_pCamDirector->SetReturnCam(CamType::Orbit);

	const OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();
	static_cast<COrbitCam*>(orbitCam)->SetTarget(curPlayer);

	CamLoader::Load();

	CameraManager()->Set_MainCam(orbitCam->Get_Component<CCamera>());
}

void CTestLevel::Ready_ShadowCamera()
{
	constexpr _float aspect = (_float)g_iWinSizeX / g_iWinSizeY;

	auto shadowCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_ShadowCam" })
		.Camera(aspect)
		.Position({ 0.f, 100.f, 30.f })
		.Rotate({0.f, 0.f, 0.f})
		.Build("ShadowCam");

	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(shadowCam, {"Test_Level", "Camera_Layer"});
	CGameInstance::GetInstance()->Get_CameraMgr()->Set_ShadowCam(shadowCam->Get_Component<CCamera>());
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
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestCloud", CTestCloud::Create());
	auto testCloud = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestCloud" })
		.Build("Test_Cloud");
	
	objMgr->Add_Object(testCloud, { "Test_Level", "Etc_Layer" });
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


	CBattleSystem::GetInstance()->DestroyInstance();
	CDataBase::GetInstance()->DestroyInstance();
	m_pCamDirector->DestroyInstance();
	m_pGameInstance->DestroyInstance();
}
