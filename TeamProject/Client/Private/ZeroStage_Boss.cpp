#include "pch.h"
#include "ZeroStage_Boss.h"
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


CZeroStage_Boss::CZeroStage_Boss()
{
}

HRESULT CZeroStage_Boss::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	return S_OK;
}

HRESULT CZeroStage_Boss::Awake()
{

	//============== Map ============================
	Ready_Map("Test_Level", "Zero_Boss1");

	return S_OK;
}

void CZeroStage_Boss::Update()
{
}

HRESULT CZeroStage_Boss::Ready_Stage(CZero_Level::StageContext& context)
{
	CMapLoader* pMapLoader = CMapLoader::Create("Zero_Level", "Zero_Boss1");
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
	return S_OK;
}

HRESULT CZeroStage_Boss::Enter_Stage(CZero_Level::StageContext& context)
{
	
	return S_OK;
}

HRESULT CZeroStage_Boss::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

void CZeroStage_Boss::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	//// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();
	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
}

void CZeroStage_Boss::Rake_MapResources()
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

void CZeroStage_Boss::Ready_Camera()
{
	constexpr _float aspect = (_float)g_iWinSizeX / g_iWinSizeY;

	auto seqCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_SequenceCam" })
		.Camera(aspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("SequenceCam");

	auto freeCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_FreeCam" })
		.Camera(aspect)
		.Position({ 0.f, 2.f, -3.f })
		.Build("FreeCam");

	CCT_DESC desc;
	desc.eGroup = COLLISION_GROUP::CAMERA;
	desc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);

	auto orbitCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_OrbitCam" })
		.Camera(aspect)
		.CharacterController(desc)
		.Build("OrbitCam");

	ObjectManager()->Add_Object(seqCam, { "Test_Level", "Camera_Layer" });
	ObjectManager()->Add_Object(freeCam, { "Test_Level", "Camera_Layer" });
	ObjectManager()->Add_Object(orbitCam, { "Test_Level", "Camera_Layer" });

	m_pCamDirector->SetCam(CamType::Sequence, seqCam->Get_Handle());
	m_pCamDirector->SetCam(CamType::Free, freeCam->Get_Handle());
	m_pCamDirector->SetCam(CamType::Orbit, orbitCam->Get_Handle());

	m_pCamDirector->SetReturnCam(CamType::Orbit);

	const OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();
	static_cast<COrbitCam*>(orbitCam)->SetTarget(curPlayer);

	CamLoader::Load();

	CameraManager()->Set_MainCam(orbitCam->Get_Component<CCamera>());
}

void CZeroStage_Boss::Ready_ShadowCamera()
{
	constexpr _float aspect = (_float)g_iWinSizeX / g_iWinSizeY;

	auto shadowCam = Builder::Create_Object({ "Test_Level", "Proto_GameObject_ShadowCam" })
		.Camera(aspect)
		.Position({ 0.f, 100.f, 30.f })
		.Rotate({ 0.f, 0.f, 0.f })
		.Build("ShadowCam");

	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(shadowCam, { "Test_Level", "Camera_Layer" });
	CGameInstance::GetInstance()->Get_CameraMgr()->Set_ShadowCam(shadowCam->Get_Component<CCamera>());
}

CZeroStage_Boss* CZeroStage_Boss::Create( CZero_Level* pOwnerLevel)
{
	CZeroStage_Boss* pInstance = new CZeroStage_Boss();
	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CZeroStage_Boss::Free()
{
	__super::Free();
}