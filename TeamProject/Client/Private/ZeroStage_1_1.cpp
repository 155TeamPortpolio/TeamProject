#include "pch.h"
#include "ZeroStage_1_1.h"
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



CZeroStage_1_1::CZeroStage_1_1()
{
}

HRESULT CZeroStage_1_1::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	CBattleSystem::GetInstance()->SetActive(true);

	return S_OK;
}

HRESULT CZeroStage_1_1::Awake()
{

	//============== Map ============================
	Ready_Map("ZeroStage_1_1", "Zero_1_1");

	return S_OK;
}

void CZeroStage_1_1::Update()
{
}

HRESULT CZeroStage_1_1::Ready_Stage(CZero_Level::StageContext& context)
{
	CMapLoader* pMapLoader = CMapLoader::Create("Zero_Level", "Zero_Boss1");

	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");

	Safe_Release(pMapLoader);
	return S_OK;
}

HRESULT CZeroStage_1_1::Enter_Stage(CZero_Level::StageContext& context)
{

	return S_OK;
}

HRESULT CZeroStage_1_1::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

void CZeroStage_1_1::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	//// Ready MapObject key and path to ResourceMgr 
	Rake_MapResources();
	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
}

void CZeroStage_1_1::Rake_MapResources()
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

CZeroStage_1_1* CZeroStage_1_1::Create(CZero_Level* pOwnerLevel)
{
	CZeroStage_1_1* pInstance = new CZeroStage_1_1();
	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CZeroStage_1_1::Free()
{
	__super::Free();
}