#include "pch.h"
#include "ZeroStage_Normal.h"
#include "Zero_Level.h"
#include "BattleSystem.h"

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
}

HRESULT CZeroStage_Normal::Ready_Stage(CZero_Level::StageContext& context)
{
	Ready_Map("Zero_Level", "Zero_1_1");
	return S_OK;
}

HRESULT CZeroStage_Normal::Enter_Stage(CZero_Level::StageContext& context)
{
	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -0.18f, 0.f,1.59f });
	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -0.18f, 0.f,1.59f });
	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -0.18f, 0.f,1.59f });
	CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_ThugBulkyEnforcer", { -0.18f, 0.f,1.59f });

	return S_OK;
}

HRESULT CZeroStage_Normal::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
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