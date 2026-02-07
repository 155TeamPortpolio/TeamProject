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
#include "Defiler.h"
#include "EnemyAttackCollider.h"
#include "EnemyTriggerCollider.h"
#include "StageRouter.h"
#include "StrikeJaeger.h"

/* UI */
#include "UIDirector.h"

/* Interactable */
#include "ZeroPortal.h"

/*Map*/
#include "MapLoader.h"
#include "DataBase.h"

CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey)
{
}

HRESULT CZero_Level::Initialize()
{
	Ready_Prototype();

	/* UI */
	CUIDirector::GetInstance()->Load_LevelObjects("Zero_Level");

	/*ENV*/
	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(false);
	RenderSystem()->Set_FogDesc({ _float4(0.08f, 0.02f, 0.02f, 1.0f),0.f, 0.f, 0.02f, true });

	/* Player */
	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	castedPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);

	m_Context.isFirstIn = true;
	m_Context.hPlayer = castedPlayer->Get_CurCharacterHandle();
	Ready_Stage();

	{
		auto pEnviromentEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_enviroment.json")
			.Build("Enviroment_Particle");

		ObjectManager()->Add_Object(pEnviromentEffect, { "Zero_Level","Effect_Layer" });
	}
	
	return S_OK;
}

HRESULT CZero_Level::Awake()
{
	if (!m_Context.hPlayer.isValid())
		return E_FAIL;

	return S_OK;
}

void CZero_Level::Update()
{
	m_Context.pNowStage->Update();
}

HRESULT CZero_Level::Render()
{
	return S_OK; 
}

HRESULT CZero_Level::ChangeStage(StageType type)
{
	auto found = m_StageContainer.find(type);
	if (found == m_StageContainer.end())
		return E_FAIL;

	if (m_Context.pNowStage)
		m_Context.pNowStage->Exit_Stage(m_Context);

	m_Context.nextType = type;
	m_Context.mapKey = PopMapKey(type);

	m_Context.pNowStage = found->second;
	return m_Context.pNowStage->Enter_Stage(m_Context);
}

void CZero_Level::Ready_Prototype()
{
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Sacrifice", CSacrifice::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeHand", CSacrificeHand::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeLaser", CSacrifice_Laser::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeOrb", CSacrifice_Orb::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugBulkyEnforcer", CThugBulkyEnforcer::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugAssaulter", CThugAssaulter::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_EnemyTriggerCollider", CEnemyTriggerCollider::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ZeroPortal", CZeroPortal::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_LevelObject_ZeroPortal", CStageRouter::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_StrikeJaeger", CStrikeJaeger::Create());
}

void CZero_Level::Ready_Stage()
{
	/*Stage*/
	m_pRouter = Add_LevelObject<CStageRouter>();Safe_AddRef(m_pRouter);
	ObjectManager()->Add_Object(m_pRouter, { "Zero_Level","Router_Layer" });

	m_pRouter->BuildGraph(5, StageType::Boss);

	m_StageContainer.emplace(StageType::Normal, CZeroStage_Normal::Create(this));
	m_StageContainer.emplace(StageType::Elite, CZeroStage_Elite::Create(this));
	m_StageContainer.emplace(StageType::Boss, CZeroStage_Boss::Create(this));

	m_mapCycle[StageType::Normal].maps	= { "Zero_1_1", "Zero_1_2" };
	m_mapCycle[StageType::Elite].maps	= { "Zero_1_1", "Zero_1_2" };
	m_mapCycle[StageType::Boss].maps	= { "Zero_Boss1","Zero_Boss2"};

	ChangeStage(StageType::Normal);
}

string CZero_Level::PopMapKey(StageType type)
{
	auto it = m_mapCycle.find(type);
	if (it == m_mapCycle.end())
		return {};
	return it->second.Next();
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

	RenderSystem()->Set_FogDesc({ _float4(0.08f, 0.02f, 0.02f, 1.0f),0.f, 0.f, 0.02f, false });

	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	castedPlayer->Clear_Characters();
}
