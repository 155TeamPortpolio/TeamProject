#include "pch.h"
#include "ZeroStage_Elite.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Zero_Level.h"
#include "BattlePlayer.h"
#include "StageRouter.h"

CZeroStage_Elite::CZeroStage_Elite()
{
	m_eType = StageType::Elite;
}

HRESULT CZeroStage_Elite::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;
	return S_OK;
}

HRESULT CZeroStage_Elite::Awake()
{
	return S_OK;
}

void CZeroStage_Elite::Update()
{
	float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);
	switch (m_eStageState)
	{
	case Client::CStage::StageState::Entrance:
		m_introFlow.Tick(dt);
		Intro();
		break;
	case Client::CStage::StageState::BattleStart:
		BattleStart();
		break;
	case Client::CStage::StageState::Battle:
		Battle();
		break;
	case Client::CStage::StageState::BattleEnd:
		m_ClearFlow.Tick(dt);
		BattleEnd();
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

HRESULT CZeroStage_Elite::Enter_Stage(StageContext& context)
{
	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageState = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;
	BaseIntro(context);
	Active_Player(CStage::PlayerPoint::Typical);

	m_pOwnerLevel->Get_ZeroFog()->Set_BaseFog(
		{
			_float4{ 0.08f, 0.05f, 0.04f, 1.0f },
			0.02f
		});

	return S_OK;
}

void CZeroStage_Elite::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		BattleSystem()->GetBattlePlayer()->UnLock_Input();
		if (!HasBattleStarter())
			m_eStageState = StageState::BattleStart;
	}
}

void CZeroStage_Elite::BattleStart()
{
	Active_Enemy();
	CBattleSystem::GetInstance()->SetActive(true);
	m_eStageState = StageState::Battle;
}

void CZeroStage_Elite::Battle()
{
	_bool isBattleEnd = CBattleSystem::GetInstance()->isMonsterCleared();
	if (isBattleEnd) {
		if (!m_MonsterQueue.empty()) {
			Active_Enemy();
			return;
		}

		WipeOutFX();
		m_eStageState = StageState::BattleEnd;
	}
}


void CZeroStage_Elite::BattleEnd()
{
	if (m_ClearFlow.IsDoneAll())
	{
		m_eStageState = StageState::Outro;
		CBattleSystem::GetInstance()->SetActive(false);
		Active_Portal();
	}
}

void CZeroStage_Elite::Outro()
{
	BaseOutro();
	m_outroFlow.Start();
	m_eStageState = StageState::End;
}

void CZeroStage_Elite::End()
{
	if (m_outroFlow.IsDoneAll()) {
		auto stageType = m_pOwnerLevel->Get_Router()->GetChoiceType(m_iNextChoice);

		//ReCycle
		m_pOwnerLevel->Get_Router()->BuildGraph(2, StageType::Rest, StageType::Boss);
		m_pOwnerLevel->ChangeStage(StageType::Rest);
	}
}

CZeroStage_Elite* CZeroStage_Elite::Create(CZero_Level* pOwnerLevel)
{
	CZeroStage_Elite* pInstance = new CZeroStage_Elite;
	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CZeroStage_Elite::Free()
{
	__super::Free();
}