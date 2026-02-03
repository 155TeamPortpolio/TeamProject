#include "pch.h"
#include "ZeroStage_Normal.h"
#include "Zero_Level.h"
#include "BattleSystem.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"
#include "UIDirector.h"
#include "StageRouter.h"

CZeroStage_Normal::CZeroStage_Normal()
{
	m_eType = StageType::Normal;
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

HRESULT CZeroStage_Normal::Enter_Stage(StageContext& context)
{

	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageStage = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;
	Active_Player(CStage::PlayerPoint::Typical);
	BaseIntro(context);

	return S_OK;
}

void CZeroStage_Normal::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		Active_Enemy();
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
		Active_Portal();
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
		auto stageType = m_pOwnerLevel->Get_Router()->GetChoiceType(m_iNextChoice);
		m_pOwnerLevel->Get_Router()->Choose(m_iNextChoice);
		m_pOwnerLevel->ChangeStage(stageType);
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