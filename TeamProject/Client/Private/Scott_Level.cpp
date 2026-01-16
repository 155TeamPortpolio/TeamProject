#include "pch.h"
#include "Scott_Level.h"
#include "Helper_Func.h"
#include "GameInstance.h"

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

/* UI */
#include "UIDirector.h"
#include "UI_MeshBillboard.h"


CScott_Level::CScott_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CScott_Level::Initialize()
{
	CBattleSystem::GetInstance()->SetActive(true);
	RenderSystem()->Set_FogDesc({ _float4(0.12f, 0.25f, 0.35f, 1.0f),0.f, 0.f, 0.005f, true });
	
	m_pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
	m_pPlayer->Set_PlayerType(CPlayer::PLAYER::FIELD);

	return S_OK;
}

HRESULT CScott_Level::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	IResourceService* pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	auto objMgr = m_pGameInstance->Get_ObjectMgr();

	// ============ Camera ==================================================
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_OrbitCam", COrbitCam::Create());
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_FreeCam", CFreeCam::Create());
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_SequenceCam", CSequenceCam::Create());
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_ShadowCam", CShadowCam::Create());
	// =========================================================================

	//============== Test =================================
	//pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestPlane", CTestPlane::Create());
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_TestFloor", CTestFloor::Create());
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_TestMap", CTestMap::Create());

	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Scott_Level");

	//============== Map ============================
	Ready_Map("Scott_Level", "Zero_Worksite");

	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_MeshBillboard", CUI_MeshBillboard::Create());


	//m_pCamDirector->SetSpaceRef(CBattleSystem::GetInstance()->GetCurCharacterHandle());
	//m_pCamDirector->RequestSequence("Jane_Intro", 0.f, true, 0.5f);

	return S_OK;
}

void CScott_Level::Update()
{
	
}

HRESULT CScott_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CScott_Level::PreLoad_Level()
{
}

void CScott_Level::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	//// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();
	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
}

void CScott_Level::Rake_MapResources()
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

CScott_Level* CScott_Level::Create(const string& LevelKey)
{
	CScott_Level* instance = new CScott_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CScott_Level::Free()
{
	__super::Free();
	CBattleSystem::GetInstance()->DestroyInstance();
	CDataBase::GetInstance()->DestroyInstance();
	m_pCamDirector->DestroyInstance();
	m_pGameInstance->DestroyInstance();
	m_pPlayer->Clear_Characters();
}
