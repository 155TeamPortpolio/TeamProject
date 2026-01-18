#include "pch.h"
#include "ZeroStage_Normal.h"

CZeroStage_Normal::CZeroStage_Normal()
{
}

HRESULT CZeroStage_Normal::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	return S_OK;
}

HRESULT CZeroStage_Normal::Awake()
{
	return E_NOTIMPL;
}

void CZeroStage_Normal::Update()
{
}

HRESULT CZeroStage_Normal::Ready_Stage(CZero_Level::StageContext& context)
{
	return E_NOTIMPL;
}

HRESULT CZeroStage_Normal::Enter_Stage(CZero_Level::StageContext& context)
{
	return E_NOTIMPL;
}

HRESULT CZeroStage_Normal::Exit_Stage(CZero_Level::StageContext& context)
{
	return E_NOTIMPL;
}
