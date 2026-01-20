#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "IResourceService.h"

#include "TestLevel.h"
#include "LogoLevel.h"
#include "LoadingLevel.h"
#include "MainCity_Level.h"
#include "Scott_Level.h"
#include "Zero_Level.h"

#include "UIDirector.h"
#include "DataBase.h"
#include "BattleSystem.h"
#include "FieldSystem.h"

// Camera
#include "CamDirector.h"
#include "OrbitCam.h"
#include "FreeCam.h"
#include "SequenceCam.h"
#include "ShadowCam.h"
#include "CamLoader.h"

#include "MapPlacedObject.h"
#include "MapTriggerObject.h"
#include "MapInvisibleWall.h"

#include "SpriteNode.h"
#include "MeshNode.h"
#include "TrailNode.h"
#include "ParticleNode.h"
#include "EffectContainer.h"
#include "AttackSign.h"
#include "Player.h"
/* UI */
#include "UI_ScreenFade.h"
#include "UI_EnemyStatus.h"
#include "UI_MeshPyramid.h"
#include "UI_MeshBillboard.h"

#include "UI_IconLabel.h" 

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	ENGINE_DESC desc{};

	desc.hWnd = g_hWnd;
	desc.eWinMode = WINMODE::WIN;
	desc.iWinSizeX = g_iWinSizeX;
	desc.iWinSizeY = g_iWinSizeY;

	if (m_pGameInstance->Init_Engine(desc)) {
		m_pDevice = m_pGameInstance->Get_Device();
		m_pDeviceContext = m_pGameInstance->Get_Context();
	}
	
	Set_Levels();

	CDataBase::GetInstance()->CreateTable();
	CBattleSystem::GetInstance();
	CDataBase::GetInstance();
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Initialize();

	CFieldSystem::GetInstance();

	Initialize_GlobalPrototype();
	Create_GlobalPlayer();
	Create_GlobalCamObjs();
	
	#ifdef  _USING_GUI
		ImGui::SetCurrentContext(m_pGameInstance->Get_GUISystem()->GetEngineImGuiContext());
	#endif //  _USING_GUI

	m_cursorController.Initialize();

	return S_OK;
}

void CMainApp::Update(const float dt)
{
	m_pGameInstance->Update_Engine(dt);
	CBattleSystem::GetInstance()->Update();
	CCamDirector::GetInstance()->Update(dt);

	m_cursorController.Update(dt);
}

HRESULT CMainApp::Render()
{
	_float4 color = { 0.f,0.f,0.f,1.f };
	m_pGameInstance->Draw_Begin(&color);
	m_pGameInstance->Draw();
	m_pGameInstance->Draw_End();
	return S_OK;
}

void CMainApp::Set_Levels()
{
	LevelManager()->Register_Level("Test_Level",     []()->CLevel* {return CTestLevel::Create("Test_Level"); });
	LevelManager()->Register_Level("Logo_Level",     []()->CLevel* {return CLogoLevel::Create("Logo_Level"); });
	LevelManager()->Register_Level("Loading_Level",  []()->CLevel* {return CLoadingLevel::Create("Loading_Level"); });
	LevelManager()->Register_Level("MainCity_Level", []()->CLevel* {return CMainCity_Level::Create("MainCity_Level"); });
	LevelManager()->Register_Level("Scott_Level",    []()->CLevel* {return CScott_Level::Create("Scott_Level"); });
	LevelManager()->Register_Level("Zero_Level",     []()->CLevel* {return CZero_Level::Create("Zero_Level"); });

	LevelManager()->Set_LoadingLevel("Loading_Level");
	m_pGameInstance->Notify_LevelSet(); 
	m_pGameInstance->Get_LevelMgr()->Request_ChangeLevel("Scott_Level",true); 
} 

CMainApp* CMainApp::Create()
{
	CMainApp* instance = new CMainApp();

	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

void CMainApp::Free()
{
	__super::Free();
	m_pGameInstance->Release_Engine();
	m_pGameInstance->DestroyInstance();
	CBattleSystem::GetInstance()->DestroyInstance();
	CUIDirector::GetInstance()->DestroyInstance();
	CCamDirector::GetInstance()->DestroyInstance();
	CDataBase::GetInstance()->DestroyInstance();
	CFieldSystem::GetInstance()->DestroyInstance();
}

void CMainApp::Initialize_GlobalPrototype()
{
	/* Prototype Tag */
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MapPlacedObject", CMapPlacedObject::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MapTriggerObject", CMapTriggerObject::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MapInvisibleWall", CMapInvisibleWall::Create());

	// Camera
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_OrbitCam",    COrbitCam::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_FreeCam",     CFreeCam::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SequenceCam", CSequenceCam::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ShadowCam",   CShadowCam::Create());

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SpriteNode", CSpriteNode::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ParticleNode", CParticleNode::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MeshNode", CMeshNode::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_TrailNode", CTrailNode::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EffectContainer", CEffectContainer::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_AttackSign", CAttackSign::Create());

	/*Player*/
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Player", CPlayer::Create());
	/* UI */
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ScreenFade", CUI_ScreenFade::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EnemyStatus", CUI_EnemyStatus::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MeshPyramid", CUI_MeshPyramid::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MeshBillboard", CUI_MeshBillboard::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_IconLabel", CUI_IconLabel::Create());
}

void CMainApp::Create_GlobalCamObjs()
{
	auto& camDirector = *CCamDirector::GetInstance();

	constexpr _float aspect = static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
	const string camLayer   = "Camera_Layer";

	auto seqCam = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_SequenceCam"})
		.Camera(aspect)
		.Position({0.f, 2.f, -5.f})
		.Build("SequenceCam");

	auto freeCam = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_FreeCam"})
		.Camera(aspect)
		.Position({0.f, 2.f, -3.f})
		.Build("FreeCam");

	CCT_DESC desc;
	desc.eGroup = COLLISION_GROUP::CAMERA;
	desc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);

	auto orbitCam = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_OrbitCam"})
		.Camera(aspect)
		.CharacterController(desc)
		.Build("OrbitCam");

	auto shadowCam = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_ShadowCam"})
		.Camera(aspect)
		.Position({0.f, 100.f, 30.f})
		.Rotate({0.f, 0.f, 0.f})
		.Build("ShadowCam");

	ObjectManager()->Add_Object(seqCam,    {G_GlobalLevelKey, camLayer});
	ObjectManager()->Add_Object(freeCam,   {G_GlobalLevelKey, camLayer});
	ObjectManager()->Add_Object(orbitCam,  {G_GlobalLevelKey, camLayer});
	ObjectManager()->Add_Object(shadowCam, {G_GlobalLevelKey, camLayer});

	ObjectManager()->Remember_Global(ENUM(GLOBAL_ID::FreeCam),   freeCam->Get_Handle(),   true);
	ObjectManager()->Remember_Global(ENUM(GLOBAL_ID::OrbitCam),  orbitCam->Get_Handle(),  true);
	ObjectManager()->Remember_Global(ENUM(GLOBAL_ID::SeqCam)  ,  seqCam->Get_Handle(),    true);
	ObjectManager()->Remember_Global(ENUM(GLOBAL_ID::ShadowCam), shadowCam->Get_Handle(), true);

	camDirector.SetCam(CamType::Sequence, seqCam->Get_Handle());
	camDirector.SetCam(CamType::Free,     freeCam->Get_Handle());
	camDirector.SetCam(CamType::Orbit,    orbitCam->Get_Handle());

	camDirector.SetReturnCam(CamType::Orbit);

	CameraManager()->Set_MainCam(orbitCam->Get_Component<CCamera>());
	CameraManager()->Set_ShadowCam(shadowCam->Get_Component<CCamera>());

	CamLoader::Load();
}

void CMainApp::Create_GlobalPlayer()
{
	auto Player = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_Player" })
		.Build("Player");
	ObjectManager()->Add_Object(Player, { G_GlobalLevelKey, "Player_Layer" });

	ObjectManager()->Remember_Global(ENUM(GLOBAL_ID::Player), Player->Get_Handle(), false);
}