#include "pch.h"
#include "ZeroStage_Elite.h"
#include "Zero_Level.h"
#include "BattleSystem.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"

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

	switch (m_eStageStage)
	{
	case Client::CStage::StageState::None:
		break;
	case Client::CStage::StageState::Entrance:
		m_radialDt += GameInstance()->Get_EngineDeltaTime();
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
	RenderSystem()->Apply_RadialBlur(2.f);

	if(context.isFirstIn){
		CCamDirector::GetInstance()->SetTarget(context.hPlayer);
		CCamDirector::GetInstance()->RequestSequence("Intro/Jane_Intro");
	}

	return S_OK;
}

HRESULT CZeroStage_Elite::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

void CZeroStage_Elite::Intro()
{
	if (m_radialDt > 2.f) {
		m_isSequenceEnd = true;
	}

	if (m_isSequenceEnd) {
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
	m_pOwnerLevel->ChangeStage(CZero_Level::StageType::Boss, 0);
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