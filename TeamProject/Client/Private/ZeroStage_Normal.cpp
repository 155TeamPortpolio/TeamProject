#include "pch.h"
#include "ZeroStage_Normal.h"
#include "Zero_Level.h"

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
	m_pOwnerLevel->Ready_Map("Zero_Level", "Zero_1_1");
	return S_OK;
}

HRESULT CZeroStage_Normal::Enter_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}

HRESULT CZeroStage_Normal::Exit_Stage(CZero_Level::StageContext& context)
{
	return S_OK;
}
