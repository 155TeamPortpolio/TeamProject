#include "pch.h"
#include "ZeroStage_Boss.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Zero_Level.h"
#include "StageRouter.h"
#include "EffectContainer.h"

CZeroStage_Boss::CZeroStage_Boss()
{
	m_eType = StageType::Boss;
}

HRESULT CZeroStage_Boss::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;
	return S_OK;
}

HRESULT CZeroStage_Boss::Awake()
{
	return S_OK;
}

void CZeroStage_Boss::Update()
{
	float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);

	switch (m_eStageStage)
	{
	case Client::CStage::StageState::Entrance:
		m_introFlow.Tick(dt);
		Intro();
		break;
	case Client::CStage::StageState::BattleStart:
		Battle();
		break;
	case Client::CStage::StageState::BattleEnd:
		break;
	case Client::CStage::StageState::Outro:
		Outro();
		break;
	case Client::CStage::StageState::End:
		m_outroFlow.Tick(dt);
		End();
		break;
	default:
		break;
	}
}


HRESULT CZeroStage_Boss::Enter_Stage(StageContext& context)
{
	auto fog = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("defiler_stage_fog.json")
		.Position(_vector3(-1.f, -3.5f, 1.8f))
		.Build("Stage_Fog");

	//ObjectManager()->Add_Object(fog, { "Zero_Level","Effect_Layer" });

	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageStage = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;

	Active_Player(CStage::PlayerPoint::Typical);
	BossIntro(context);
	return S_OK;
}

void CZeroStage_Boss::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		Active_Enemy();
		CBattleSystem::GetInstance()->SetActive(true);
		m_eStageStage = StageState::BattleStart;
	}
}

void CZeroStage_Boss::Battle()
{
	_bool isBattleEnd = CBattleSystem::GetInstance()->isMonsterCleared();
	if (isBattleEnd) {                 
		m_eStageStage = StageState::Outro;
		CBattleSystem::GetInstance()->SetActive(false);
	}
}


void CZeroStage_Boss::Outro()
{
	BaseOutro();
	m_outroFlow.Start();
	m_eStageStage = StageState::End;
}

void CZeroStage_Boss::End()
{
	if (m_outroFlow.IsDoneAll()) {
		LevelManager()->Request_ChangeLevel("Scott_Level", true);
	}
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