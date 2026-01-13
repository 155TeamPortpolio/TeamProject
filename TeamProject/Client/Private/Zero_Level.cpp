#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "IStage.h"
#include "ZeroStage_Boss.h"

CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CZero_Level::Initialize()
{
	m_StageContainer.emplace(StageType::Boss, CZeroStage_Boss::Create(this));
	return S_OK;
}

HRESULT CZero_Level::Awake()
{
	return S_OK;
}

void CZero_Level::Update()
{
	
}

HRESULT CZero_Level::Render()
{
	return S_OK;
}

void CZero_Level::PreLoad_Level()
{
	/*여기에 Add ResourcePath 넣기*/
}

CZero_Level* CZero_Level::Create(const string& LevelKey)
{
	CZero_Level* instance = new CZero_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CZero_Level::Free()
{
	__super::Free();
	m_pGameInstance->DestroyInstance();
}
