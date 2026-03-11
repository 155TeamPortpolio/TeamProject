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
#include "Gacha_Level.h"
#include "Tutorial_Level.h"

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

// Map
#include "MapPlacedObject.h"
#include "MapTriggerObject.h"
#include "MapInvisibleWall.h"
#include "XWall.h"
#include "MapLightPoint.h"

#include "SpriteNode.h"
#include "MeshNode.h"
#include "TrailNode.h"
#include "ParticleNode.h"
#include "EffectContainer.h"
#include "AttackSign.h"
#include "Player.h"
#include "TestCloud.h"
#include "BasicHitEffect.h"
#include "ProceduralSky.h"
#include "AttackRange.h"
#include "JaegerLaser.h"

/* UI */
#include "ButtonUI.h"
#include "CanvasPanel.h"
#include "GaugeUI.h"
#include "ImageUI.h"
#include "MaskUI.h"
#include "NineSliceUI.h"
#include "SoftDirectionalOutlineUI.h"
#include "SpriteAnimationUI.h"
#include "TextUI.h"
#include "UVAnimationUI.h" 

#include "UI_ButtonPanel.h"
#include "UI_BackButton.h"
#include "UI_IconButton.h"
#include "UI_TextButton.h"
#include "UI_Banner.h"

#include "UI_Logo.h"
#include "UI_Loading.h"
#include "UI_ScreenFade.h"
#include "UI_SceneFrame.h"

#include "UI_Mouse.h"

#include "UI_NameIndicator.h" 
#include "UI_SpeechBubble.h"

#include "UI_BattleHUD.h"
#include "UI_FieldHUD.h"
#include "UI_BossHUD.h" 
#include "UI_EnemyStatus.h"
#include "UI_Switch.h"
#include "UI_MeshPyramid.h"
#include "UI_AtlasSprite.h"
#include "UI_DamageText.h"
#include "UI_Gangta.h"
#include "UI_Seoriyeol.h"

#include "UI_Dialogue.h" 
#include "UI_Lottery.h"
#include "UI_Ramen.h" 
#include "UI_GachaPage.h" 

#include "UI_GachaResult.h"
#include "VideoPanel.h"
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

	CFieldSystem::GetInstance();

	Initialize_GlobalPrototype();
	Create_GlobalPlayer();
	Create_GlobalCamObjs();
	Create_GlobalEnviroment();

	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Initialize();
	#ifdef  _USING_GUI
		ImGui::SetCurrentContext(m_pGameInstance->Get_GUISystem()->GetEngineImGuiContext());
		//GUISystem()->Register_Panel(CVideoPanel::Create(GUISystem()->Get_Context()));
	#endif //  _USING_GUI

	m_cursorController.Initialize();

	ShowCursor(true);

	return S_OK;
}

void CMainApp::Update(const float dt)
{
	m_pGameInstance->Update_Engine(dt);
	CamDirector()->Update(dt); 

#ifdef NDEBUG
	m_cursorController.Update(dt);
#endif
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
	LevelManager()->Register_Level("Test_Level",			[]()->CLevel* {return CTestLevel::Create("Test_Level"); });
	LevelManager()->Register_Level("Logo_Level",			[]()->CLevel* {return CLogoLevel::Create("Logo_Level"); });
	LevelManager()->Register_Level("Loading_Level",			[]()->CLevel* {return CLoadingLevel::Create("Loading_Level"); });
	LevelManager()->Register_Level("MainCity_Level",		[]()->CLevel* {return CMainCity_Level::Create("MainCity_Level"); });
	LevelManager()->Register_Level("Scott_Level",			[]()->CLevel* {return CScott_Level::Create("Scott_Level"); });
	LevelManager()->Register_Level("Zero_Level",			[]()->CLevel* {return CZero_Level::Create("Zero_Level"); });
	LevelManager()->Register_Level("Gacha_Level",			[]()->CLevel* {return CGacha_Level::Create("Gacha_Level"); });
	LevelManager()->Register_Level("Tutorial_Level",		[]()->CLevel* {return CTutorial_Level::Create("Tutorial_Level"); });

	LevelManager()->Set_LoadingLevel("Loading_Level");
	m_pGameInstance->Notify_LevelSet(); 
	m_pGameInstance->Get_LevelMgr()->Request_ChangeLevel("Test_Level", true);

	//m_pGameInstance->Get_LevelMgr()->Request_ChangeLevel("Zero_Level", true);
	
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
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_XWall", CXWall::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MapLightPoint", CMapLightPoint::Create());

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
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BasicHitEffect", CBasicHitEffect::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_AttackRange", CAttackRange::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_JaegerLaser", CJaegerLaser::Create());

	/*Player*/
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Player", CPlayer::Create());
	/* UI */
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Button", CButtonUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_CanvasPanel", CCanvasPanel::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Gauge", CGaugeUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Image", CImageUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SoftDirectionalOutline", CSoftDirectionalOutlineUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SpriteAnimation", CSpriteAnimationUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Text", CTextUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_UVAnimation", CUVAnimationUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Mask", CMaskUI::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_NineSlice", CNineSliceUI::Create());

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ButtonPanel", CUI_ButtonPanel::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BackButton", CUI_BackButton::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_IconButton", CUI_IconButton::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_TextButton", CUI_TextButton::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Banner", CUI_Banner::Create());

	ResourceManager()->Add_ResourcePath("empty.png", "../Bin/Resources/Global/UI/Image/empty.png");
	ResourceManager()->Add_ResourcePath("Run2.png", "../Bin/Resources/Global/UI/Image/Loading/Run2.png");
	ResourceManager()->Add_ResourcePath("LoadingText.png", "../Bin/Resources/Global/UI/Image/Loading/LoadingText.png");
	ResourceManager()->Add_ResourcePath("Logo_Mi2.png", "../Bin/Resources/Global/UI/Image/Logo/Logo_Mi2.png");

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Logo", CUI_Logo::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Loading", CUI_Loading::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ScreenFade", CUI_ScreenFade::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SceneFrame", CUI_SceneFrame::Create());

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Mouse", CUI_Mouse::Create());

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_NameIndicator", CUI_NameIndicator::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SpeechBubble", CUI_SpeechBubble::Create());

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BattleHUD", CUI_BattleHUD::Create()); 
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_FieldHUD", CUI_FieldHUD::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BossHUD", CUI_BossHUD::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EnemyStatus", CUI_EnemyStatus::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Switch", CUI_Switch::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MeshPyramid", CUI_MeshPyramid::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_AtlasSprite", CUI_AtlasSprite::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_DamageText", CUI_DamageText::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Gangta", CUI_Gangta::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Seoriyeol", CUI_Seoriyeol::Create());

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Dialogue", CUI_Dialogue::Create()); 
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Lottery", CUI_Lottery::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Ramen", CUI_Ramen::Create()); 
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaPage", CUI_GachaPage::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaResult", CUI_GachaResult::Create());

	/*Enviroment*/
	ResourceManager()->Add_ResourcePath("cloud_noise.png", "../Bin/Resources/Global/Shader/cloud_noise.png");
	ResourceManager()->Add_ResourcePath("Eff_Noise_092.png", "../Bin/Resources/Global/Shader/Eff_Noise_092.png");
	ResourceManager()->Add_ResourcePath("Eff_Noise_146.png", "../Bin/Resources/Global/Shader/Eff_Noise_146.png");
	ResourceManager()->Add_ResourcePath("moon_bright.png", "../Bin/Resources/Global/Shader/moon_bright.png");
	ResourceManager()->Add_ResourcePath("moon_glow.png", "../Bin/Resources/Global/Shader/moon_glow.png");

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Cloud", CProceduralSky::Create());

}

void CMainApp::Create_GlobalCamObjs()
{
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
	desc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::INTERACTABLE) | ENUM(COLLISION_GROUP::GROUND);

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

	CamDirector()->SetCam(CamType::Sequence, seqCam->Get_Handle());
	CamDirector()->SetCam(CamType::Free,     freeCam->Get_Handle());
	CamDirector()->SetCam(CamType::Orbit,    orbitCam->Get_Handle());

	CamDirector()->SetReturnCam(CamType::Orbit);

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

void CMainApp::Create_GlobalEnviroment()
{
	auto Cloud = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_Cloud" })
		.Build("Cloud");
	ObjectManager()->Add_Object(Cloud, { G_GlobalLevelKey, "Enviroment_Layer" });

	ObjectManager()->Remember_Global(ENUM(GLOBAL_ID::Cloud), Cloud->Get_Handle(), false);
}
