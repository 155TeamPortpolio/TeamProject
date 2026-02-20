#include "pch.h"
#include "ZeroStage_Rest.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Zero_Level.h"
#include "StageRouter.h"
#include "ZeroPortal.h"

CZeroStage_Rest::CZeroStage_Rest()
{
	m_eType = StageType::Rest;
}

HRESULT CZeroStage_Rest::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;
	return S_OK;
}

HRESULT CZeroStage_Rest::Awake()
{
	return S_OK;
}

void CZeroStage_Rest::Update()
{
	float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);

	switch (m_eStageState)
	{
	case Client::CStage::StageState::Entrance:
		m_introFlow.Tick(dt);
		Intro();
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

HRESULT CZeroStage_Rest::Enter_Stage(StageContext& context)
{
	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageState = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;
	Active_Player(CStage::PlayerPoint::Typical);
	BaseIntro(context);

	return S_OK;
}

void CZeroStage_Rest::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		Active_Enemy();
		CBattleSystem::GetInstance()->SetActive(true);
		m_eStageState = StageState::None;
		Active_Portal();
	}
}

void CZeroStage_Rest::Outro()
{
	BaseOutro();
	m_eStageState = StageState::End;
}

void CZeroStage_Rest::End()
{
	if (m_outroFlow.IsDoneAll()) {
		auto stageType = m_pOwnerLevel->Get_Router()->GetChoiceType(m_iNextChoice);
		m_pOwnerLevel->Get_Router()->Choose(m_iNextChoice);
		m_pOwnerLevel->ChangeStage(stageType);
	}
}

void CZeroStage_Rest::Active_Portal()
{
	auto pRouter = m_pOwnerLevel->Get_Router();
	const int choiceCount = pRouter->GetChoiceCount();
	if (choiceCount <= 0) return;
	if (m_pPortals.empty()) return;

	for (size_t i = 0; i < 2; i++)
	{
		if (!m_pPortals[i]) continue;

		auto* zeroPortal = dynamic_cast<CZeroPortal*>(m_pPortals[i]);
		if (zeroPortal) {
			zeroPortal->Set_Alive(true);
			zeroPortal->SetChoiceIndex(this, i);
		}
	}
}

CZeroStage_Rest* CZeroStage_Rest::Create(CZero_Level* pOwnerLevel)
{
	CZeroStage_Rest* pInstance = new CZeroStage_Rest;
	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CZeroStage_Rest::Free()
{
	__super::Free();
}