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

#include "GameInstance.h"
#include "Layer.h"

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
	return S_OK;
}

void CZeroStage_Boss::Update()
{


	float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);
	m_fStageTime += dt;

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

HRESULT CZeroStage_Boss::Ready_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

HRESULT CZeroStage_Boss::Enter_Stage(CZero_Level::StageContext& context)
{
	Ready_Map("Zero_Level", "Zero_Boss1");
	m_eStageStage = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;

	CamDirector()->GetCharacter()->Get_Component<CCharacterController>()->Set_Position({0.f, 1.f, 0.f});

	BossIntro(context);
	return S_OK;
}

HRESULT CZeroStage_Boss::Exit_Stage(CZero_Level::StageContext& context)
{
	ObjectManager()->Get_Layer({ "Zero_Level","PlacedObject_Layer" })->Clear_Layer();
	return S_OK;
}

void CZeroStage_Boss::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_Sacrifice", {_vector3(-2.f, 1.f, 21.f)});
		CBattleSystem::GetInstance()->SetActive(true);
		m_eStageStage = StageState::BattleStart;
	}
}

void CZeroStage_Boss::Battle()
{
	_bool isBattleEnd = CBattleSystem::GetInstance()->isMonsterCleared();
	if (isBattleEnd) {
		m_eStageStage = StageState::BattleEnd;
		CBattleSystem::GetInstance()->SetActive(false);
		STAGE_CHANGED_DESC Stage_End = { this };
		EventSystem()->Broadcast<STAGE_CHANGED_DESC>(Stage_End);
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
		RenderSystem()->UnRegister_AddictiveColor();
		ObjectManager()->Get_Layer({ "Zero_Level","PlacedObject_Layer" })->Clear_Layer();
		ObjectManager()->Get_Layer({ "Zero_Level","InteractableObject_Layer" })->Clear_Layer();
		m_pOwnerLevel->ChangeStage(StageType::Boss, 0);
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