#include "pch.h"
#include "ZeroStage_Normal.h"
#include "Zero_Level.h"
#include "BattleSystem.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"

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
	switch (m_eStageStage)
	{
	case Client::CStage::StageState::None:
		break;
	case Client::CStage::StageState::Entrance:
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
	default:
		break;
	}
	//if (InputDevice()->Key_Tap(VK_F4))
	//{
	//
	//}
	//if (InputDevice()->Key_Tap(VK_F5))
	//{
	//	m_isSequenceEnd = true;
	//}
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

	CCamDirector::GetInstance()->SetTarget(context.hPlayer);
	CCamDirector::GetInstance()->RequestSequence("Intro/Jane_Intro");

	return S_OK;
}

HRESULT CZeroStage_Normal::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

void CZeroStage_Normal::Intro()
{
	m_isSequenceEnd = !CCamDirector::GetInstance()->IsPlaying("Intro/Jane_Intro");

	if (m_isSequenceEnd) {
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
	m_pOwnerLevel->ChangeStage(CZero_Level::StageType::Boss, 0);
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