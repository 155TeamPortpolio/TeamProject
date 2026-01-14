#include "pch.h"
#include "ZeroStage_Boss.h"
#include "MapLoader.h"

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
}

HRESULT CZeroStage_Boss::Ready_Stage(CZero_Level::StageContext& context)
{

	return S_OK;
}

HRESULT CZeroStage_Boss::Enter_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

HRESULT CZeroStage_Boss::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
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