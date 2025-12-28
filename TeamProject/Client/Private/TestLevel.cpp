#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "TestMap.h"
#include "TestObject.h"
#include "TestFloor.h"
#include "RigidBody.h"
#include "CharacterController.h"

// Camera
#include "Camera.h"
#include "FreeCam.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "SequenceCam.h"
#include "CamPanel.h"

/* MapData */
#include "MapDataCloud.h"
#include "MapLoader.h"
#include "MapPlacedObject.h"

/* Effect */
#include "MeshNode.h"
#include "SpriteNode.h"
#include "ParticleNode.h"
#include "EffectContainer.h"

/* Character */
#include "Miyabi.h"
#include "Sacrifice.h"

/* UI */
#include "ButtonUI.h"
#include "CanvasPanel.h" 
#include "GaugeUI.h"
#include "ImageUI.h"
#include "SpriteAnimationUI.h"
#include "TextUI.h"
#include "UVAnimationUI.h"

#include "HUD.h"

CTestLevel::CTestLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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

	//==============TestModel==========================
	//auto testModel = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestModel" })
	//	.CharacterController({})
	//	.Build("Test_Model");

	//objMgr->Add_Object(testModel, { "Test_Level", "Model_Layer"});

	// =================TestMap==================
	//auto testMap = Builder::Create_Object({"Test_Level", "Proto_GameObject_TestMap"})
	//	.Build("Test_Map");
	//
	//objMgr->Add_Object(testMap, {"Test_Level", "Model_Layer"});


	// =====================TestFloor=========================
	COLLIDER_DESC colDesc;
	colDesc.bCooking = true;
	colDesc.strModelKey = "Concert_Ground_FloorTile_01.model";

	for (_int z = 0; z < 3; ++z)
	{
	for (_int x = 0; x < 3; ++x)
		{
			CGameObject* pTestFloor = Builder::Create_Object({ "Test_Level", "Proto_GameObject_TestFloor" })
				.Collider(colDesc)
				.Position({ x * 6.15f, 0.f, z * 6.15f })
				.Build("Test_Floor_" + to_string(z * 3 + x));
			objMgr->Add_Object(pTestFloor, { "Test_Level", "Model_Layer" });
		}
	}


	/* Miyabi */
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Miyabi", CMiyabi::Create());
	CCT_DESC miyabiCCT;
	miyabiCCT.eGroup = COLLISION_GROUP::PLAYER;
	miyabiCCT.iCollisionMask = 0xFFFFFFFF;
	//miyabiCCT.iCollisionMask = 0xFFFFFFFF & ~(1 << ENUM(COLLISION_GROUP::COMMON));
	miyabiCCT.bAutoFit = false;
	miyabiCCT.fHeight = 1.28f;
	miyabiCCT.fRadius = 0.2f;
	miyabiCCT.eGroup = COLLISION_GROUP::PLAYER;
	//miyabiCCT.fBoundingMinY = -0.88f;
	miyabiCCT.vPos = { 0.f, 1.5f, 0.f };
	auto Miyabi = Builder::Create_Object({ "Test_Level", "Proto_GameObject_Miyabi" })
		.CharacterController(miyabiCCT)
		.Build("Miyabi");
	objMgr->Add_Object(Miyabi, { "Test_Level", "Model_Layer" });

	m_miyabiHandle = Miyabi->Get_Handle();

	/* Enemy */
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Sacrifice", CSacrifice::Create());

	// --------------------------- Camera -------------------------------------------------
	Ready_Camera();

	//==================== UI =======================
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Button", CButtonUI::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_CanvasPanel", CCanvasPanel::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Gauge", CGaugeUI::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Image", CImageUI::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_SpriteAnimation", CSpriteAnimationUI::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Text", CTextUI::Create());
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_UVAnimation", CUVAnimationUI::Create());
	//
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_HUD", CHUD::Create());
	//
	//pResource->Add_ResourcePath("PanelBox.dds", "../Bin/Resources/UI/Image/PanelBox.dds");
	//pResource->Add_ResourcePath("SkillBtnAImg.png", "../Bin/Resources/UI/Image/SkillBtnAImg.png");
	//pResource->Add_ResourcePath("SkillBtnAOutline.png", "../Bin/Resources/UI/Image/SkillBtnAOutline.png");
	//pResource->Add_ResourcePath("SkillCDMask.png", "../Bin/Resources/UI/Image/SkillCDMask.png"); 
	//pResource->Add_ResourcePath("SkillEvade.png", "../Bin/Resources/UI/Image/SkillEvade.png");
	//pResource->Add_ResourcePath("SkillSpecial.png", "../Bin/Resources/UI/Image/SkillSpecial.png");
	//pResource->Add_ResourcePath("ZeroScreenLarge02.png", "../Bin/Resources/UI/Image/ZeroScreenLarge02.png");
	//
	//m_pGameInstance->Get_FontSystem()->Add_Font("Impact.spritefont", L"../Bin/Resources/UI/Font/Impact.spritefont");
	//
	//pResource->Add_ResourcePath("hud.json", "../Bin/Resources/UI/Data/hud.json");
	//
	//CUI_Object* pUIObj = Builder::Create_UIObject({ "Test_Level", "Proto_GameObject_HUD" })
	//	.Asset("hud.json")
	//	.Build("HUD");
	//
	//m_pGameInstance->Get_UIMgr()->Add_UIObject(pUIObj, "Test_Level");




	return S_OK;
}

void CTestLevel::Update()
{
	if (KEY->Key_Down('1'))
	{
		auto obj = OBJ->Request_Object(m_freeCamHandle);
		CAM->Set_MainCam(obj->Get_Component<CCamera>(), 0.25f);
	}
	if (KEY->Key_Down('2'))
	{
		auto obj = OBJ->Request_Object(m_orbitCamHandle);
		CAM->Set_MainCam(obj->Get_Component<CCamera>(), 0.25f);
	}
	if (KEY->Key_Down('3'))
	{
		m_pCamDirector->RequestSequence("Intro", 0.f, true, 0.25f);
	}
	m_pCamDirector->Update(m_pGameInstance->Get_EngineDeltaTime());

	if (KEY->Key_Tap('4'))
	{
		CCT_DESC sacrificeCCT;
		sacrificeCCT.eGroup = COLLISION_GROUP::MONSTER;
		sacrificeCCT.iCollisionMask = 0xFFFFFFFF;
		sacrificeCCT.bAutoFit = false;
		sacrificeCCT.fHeight = 1.28f;
		sacrificeCCT.fRadius = 0.2f;
		sacrificeCCT.eGroup = COLLISION_GROUP::MONSTER;
		sacrificeCCT.vPos = { 0.f, 1.5f, 0.f };

		auto pSacrifice = Builder::Create_Object({ "Test_Level","Proto_GameObject_Sacrifice" })
			.CharacterController(sacrificeCCT)
			.Build("Sacrifice");
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pSacrifice, {"Test_Level","Enemy_Layer"});
	}
}

void CTestLevel::Ready_Camera()
{
	constexpr _float aspect = (_float)g_iWinSizeX / g_iWinSizeY;

	auto sequenceCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_SequenceCam"})
		.Camera(aspect)
		.Position({0.f, 2.f, -5.f})
		.Build("SequenceCam");

	auto freeCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_FreeCam"})
		.Camera(aspect)
		.Position({0.f, 2.f, -3.f})                                       
		.Build("FreeCam");

	CCT_DESC desc;
	desc.eGroup = COLLISION_GROUP::CAMERA;

	auto orbitCam = Builder::Create_Object({"Test_Level", "Proto_GameObject_OrbitCam"})
		.Camera(aspect)
		.CharacterController(desc)
		.Build("OrbitCam");
	static_cast<COrbitCam*>(orbitCam)->SetTarget(m_miyabiHandle.Get());

	OBJ->Add_Object(orbitCam,    {"Test_Level", "Camera_Layer"});
	OBJ->Add_Object(sequenceCam, {"Test_Level", "Camera_Layer"});
	OBJ->Add_Object(freeCam,     {"Test_Level", "Camera_Layer"});

	m_freeCamHandle  = freeCam->Get_Handle();
	m_orbitCamHandle = orbitCam->Get_Handle();
	m_seqCamHandle   = sequenceCam->Get_Handle();

	m_pCamDirector->Bind(static_cast<CSequenceCam*>(sequenceCam));
	m_pCamDirector->SetReturnCam(orbitCam->Get_Handle(), CamReturnType::OrbitCam);
	m_pCamDirector->Register("Intro", "../bin/Resources/Camera/Intro_2.cam");
	m_pCamDirector->SetSpaceReference(m_miyabiHandle);

	CAM->Set_MainCam(orbitCam->Get_Component<CCamera>());

	auto camPanel = CCamPanel::Create(GUI->Get_Context());
	GUI->Register_Panel(camPanel);

	//CAM->Set_MainCam(freeCam->Get_Component<CCamera>());
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
