#include "pch.h"
#include "ZeroStage_Elite.h"
#include "Zero_Level.h"
#include "BattleSystem.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"
#include "StageFx.h"
#include "UIDirector.h"

CZeroStage_Elite::CZeroStage_Elite()
{
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

HRESULT CZeroStage_Elite::Ready_Stage(CZero_Level::StageContext& context)
{
	Ready_Map("Zero_Level", "Zero_1_1");
	return S_OK;
}

HRESULT CZeroStage_Elite::Enter_Stage(CZero_Level::StageContext& context)
{
	Ready_Map("Zero_Level", "Zero_1_1");
	m_eStageStage = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;

	if (!m_introFlowBuilt)
	{
		m_introFlowBuilt = true;

		size_t seqId = m_introFlow.BeginSequence();

		if (context.isFirstIn)
		{
			m_introFlow.AddOnce(seqId, [context]() {if (context.isFirstIn)
			{
				CCamDirector::GetInstance()->AutoTarget();
				CCamDirector::GetInstance()->RequestSequence("Intro/Jane_Intro");
			}
				});
			m_introFlow.AddWaitUntil(seqId, []()
				{
					return !CCamDirector::GetInstance()->IsPlaying("Intro/Jane_Intro");
				});
		}
		else { 
			m_introFlow.AddWait(seqId, 0.5f);
			m_introFlow.AddOnce(seqId, [this]() {CUIDirector::GetInstance()->FadeOut_Screen(1.f); });
			m_introFlow.AddWait(seqId, 0.5f);
			m_introFlow.AddOnce(seqId, [this]() {RenderSystem()->Apply_RadialBlur(2.f); });
			m_introFlow.AddWait(seqId, 2.0f);
		}
		
		m_introFlow.EndSequence(seqId);
	}

	m_introFlow.Start();
	return S_OK;
}

HRESULT CZeroStage_Elite::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

void CZeroStage_Elite::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -13.f, -5.f,34.f });
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -1.f, -5.f,38.f });
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugAssaulter", { -12.f, -5.f,34.f });

		CBattleSystem::GetInstance()->SetActive(true);
		m_eStageStage = StageState::BattleStart;
	}
}

void CZeroStage_Elite::Battle()
{
	_bool isBattleEnd = CBattleSystem::GetInstance()->isMonsterCleared();
	if (isBattleEnd) {
		m_eStageStage = StageState::BattleEnd;
		CBattleSystem::GetInstance()->SetActive(false);
		STAGE_CHANGED_DESC Stage_End = {this};
		EventSystem()->Broadcast<STAGE_CHANGED_DESC>(Stage_End);
	}
}


void CZeroStage_Elite::Outro()
{
	if (!m_outroFlowBuilt) {
		m_outroFlowBuilt = true;

		size_t seqId = m_outroFlow.BeginSequence();
		m_outroFlow.AddOnce(seqId, [this]() {CUIDirector::GetInstance()->FadeIn_Screen(1.f); });
		m_outroFlow.AddWait(seqId, 2.0f);
	}

	m_outroFlow.Start();
	m_eStageStage = StageState::End;
}

void CZeroStage_Elite::End()
{
	if (m_outroFlow.IsDoneAll()) {
		RenderSystem()->UnRegister_AddictiveColor();
		ObjectManager()->Get_Layer({ "Zero_Level","PlacedObject_Layer" })->Clear_Layer();
		ObjectManager()->Get_Layer({ "Zero_Level","InteractableObject_Layer" })->Clear_Layer();
		m_pOwnerLevel->ChangeStage(CZero_Level::StageType::Boss, 0);
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