#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "BattleSystem.h"

#include "Helper_Func.h"
#include "EffectContainer.h"

#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "ZeroStage_Normal.h"
#include "ZeroStage_Elite.h"
#include "TestCloud.h"

// Camera
#include "Camera.h"
#include "CamDirector.h"
#include "UI_MeshPyramid.h"
#include "Player.h"

/* Enemy */
#include "Sacrifice.h" 
#include "SacrificeHand.h"
#include "Sacrifice_Laser.h"
#include "Sacrifice_Orb.h"
#include "ThugBulkyEnforcer.h"
#include "ThugAssaulter.h"
#include "EnemyAttackCollider.h"
#include "EnemyTriggerCollider.h"

/* UI */
#include "UIDirector.h"

/* Interactable */
#include "ZeroPortal.h"

/*Map*/
#include "MapLoader.h"
#include "DataBase.h"

CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{CCamDirector::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pCamDirector);
}

HRESULT CZero_Level::Initialize()
{
	/* UI */
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Zero_Level");

	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	castedPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);
	m_Context.hPlayer = castedPlayer->Get_CurCharacterHandle();

	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(false);
	/* Cloud */
	//PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_TestCloud", CTestCloud::Create());
	//auto testCloud = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_TestCloud" })
	//	.Scale(_float3(2.f, 2.f, 2.f))
	//	.Build("Test_Cloud");
	//
	//ObjectManager()->Add_Object(testCloud, {"Zero_Level", "Etc_Layer"});

	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Sacrifice", CSacrifice::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeHand", CSacrificeHand::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeLaser", CSacrifice_Laser::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeOrb", CSacrifice_Orb::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugBulkyEnforcer", CThugBulkyEnforcer::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugAssaulter", CThugAssaulter::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_EnemyTriggerCollider", CEnemyTriggerCollider::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ZeroPortal", CZeroPortal::Create());
	RenderSystem()->Set_FogDesc({ _float4(0.08f, 0.02f, 0.02f, 1.0f),0.f, 0.f, 0.02f, true });

	m_StageContainer.emplace(StageType::Boss, CZeroStage_Boss::Create(this));
	m_StageContainer.emplace(StageType::Normal, CZeroStage_Normal::Create(this));
	m_StageContainer.emplace(StageType::Elite, CZeroStage_Elite::Create(this));

	m_Context.isFirstIn = true;
	ChangeStage(StageType::Boss, 1);
	m_Context.isFirstIn = false;
	//m_Context.pNowStage->Ready_Stage(m_Context);

	{
		auto pResource = ResourceManager();
		auto pEnviromentEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_enviroment.json")
			.Build("Enviroment_Particle");

		ObjectManager()->Add_Object(pEnviromentEffect, { "Zero_Level","Effect_Layer" });
	}
	
	return S_OK;
}

HRESULT CZero_Level::Awake()
{
	/* Enemy */
	/* Player */
	if (!m_Context.hPlayer.isValid())
		return E_FAIL;
	return S_OK;
}

void CZero_Level::Update()
{
	m_Context.pNowStage->Update();

	if (InputDevice()->Key_Tap(VK_F4))
	{
		ChangeStage(StageType::Boss, 0);
	}

}

HRESULT CZero_Level::Render()
{
	return S_OK;
}

void CZero_Level::PreLoad_Level()
{
	
}

HRESULT CZero_Level::ChangeStage(StageType nextStageType, _int StageID)
{
	if (m_Context.eStageType == nextStageType && m_Context.pNowStage)
		return S_OK;

	if (m_Context.pNowStage)
		m_Context.pNowStage->Exit_Stage(m_Context);

	auto found = m_StageContainer.find(nextStageType);
	if (found == m_StageContainer.end())
		return E_FAIL;

	m_Context.eStageType = nextStageType;
	m_Context.StageID = StageID;
	m_Context.pNowStage = found->second;

	return m_Context.pNowStage->Enter_Stage(m_Context);
}

CZero_Level* CZero_Level::Create(const string& LevelKey)
{
	CZero_Level* instance = new CZero_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CZero_Level::Free()
{
	__super::Free();

	m_Context.pNowStage = nullptr;
	for (auto& pair : m_StageContainer)
		Safe_Release(pair.second);
	m_StageContainer.clear();

	m_pGameInstance->DestroyInstance();
	m_pCamDirector->DestroyInstance();

	RenderSystem()->Set_FogDesc({ _float4(0.08f, 0.02f, 0.02f, 1.0f),0.f, 0.f, 0.02f, false });

	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	castedPlayer->Clear_Characters();
}
