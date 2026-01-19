#include "pch.h"
#include "ZeroStage_Normal.h"
#include "Zero_Level.h"
#include "BattleSystem.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"
#include "UIDirector.h"

CZeroStage_Normal::CZeroStage_Normal()
{
}

HRESULT CZeroStage_Normal::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;
	return S_OK;
}

HRESULT CZeroStage_Normal::Awake()
{
	return S_OK;
}

void CZeroStage_Normal::Update()
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

HRESULT CZeroStage_Normal::Ready_Stage(CZero_Level::StageContext& context)
{
	Ready_Map("Zero_Level", "Zero_1_1");
	return S_OK;
}

HRESULT CZeroStage_Normal::Enter_Stage(CZero_Level::StageContext& context)
{
	Ready_Map("Zero_Level", "Zero_1_1");
	m_eStageStage = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;
	BaseIntro(context);
	return S_OK;
}

HRESULT CZeroStage_Normal::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

void CZeroStage_Normal::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -13.f, -5.f,34.f });
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -1.f, -5.f,38.f });
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -12.f, -5.f,34.f });

		CBattleSystem::GetInstance()->SetActive(true);
		m_eStageStage = StageState::BattleStart;
	}
}

void CZeroStage_Normal::Battle()
{
	_bool isBattleEnd = CBattleSystem::GetInstance()->isMonsterCleared();
	if (isBattleEnd) {
		m_eStageStage = StageState::BattleEnd;
		CBattleSystem::GetInstance()->SetActive(false);
		STAGE_CHANGED_DESC Stage_End = {this};
		EventSystem()->Broadcast<STAGE_CHANGED_DESC>(Stage_End);
	}
}

void CZeroStage_Normal::Outro()
{
	BaseOutro();
	m_eStageStage = StageState::End;
}

void CZeroStage_Normal::End()
{
	if (m_outroFlow.IsDoneAll()) {
		RenderSystem()->UnRegister_AddictiveColor();
		ObjectManager()->Get_Layer({ "Zero_Level","PlacedObject_Layer" })->Clear_Layer();
		ObjectManager()->Get_Layer({ "Zero_Level","InteractableObject_Layer" })->Clear_Layer();
		m_pOwnerLevel->ChangeStage(CZero_Level::StageType::Elite, 0);
	}
}


CZeroStage_Normal* CZeroStage_Normal::Create(CZero_Level* pOwnerLevel)
{
	CZeroStage_Normal* pInstance = new CZeroStage_Normal;
	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CZeroStage_Normal::Free()
{
	__super::Free();
}