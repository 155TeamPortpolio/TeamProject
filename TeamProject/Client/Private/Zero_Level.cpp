#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Stage.h"
#include "ZeroStage_Boss.h"

CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CZero_Level::Initialize()
{
	auto boss = CZeroStage_Boss::Create(this);
	m_StageContainer.emplace(StageType::Boss, boss);

	m_Context.eStageType = StageType::Boss;
	m_Context.pNowStage = boss;

	return S_OK;
}

HRESULT CZero_Level::Awake()
{
	return S_OK;
}

void CZero_Level::Update()
{
	m_Context.pNowStage->Update();
}

HRESULT CZero_Level::Render()
{
	return S_OK;
}

HRESULT CZero_Level::ChangeStage(StageType nextStageType)
{
	if (m_Context.eStageType == nextStageType && m_Context.pNowStage)
		return S_OK;

	if (m_Context.pNowStage)
		m_Context.pNowStage->Exit_Stage(m_Context);

	auto found = m_StageContainer.find(nextStageType);
	if (found == m_StageContainer.end())
		return E_FAIL;

	m_Context.eStageType = nextStageType;
	m_Context.pNowStage = found->second;

	return m_Context.pNowStage->Enter_Stage(m_Context);
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
	for (auto& pair : m_StageContainer)
		Safe_Release(pair.second);
	m_StageContainer.clear();

	Safe_Release(m_Context.pNowStage);
	m_Context.pNowStage = nullptr;

	__super::Free();
	m_pGameInstance->DestroyInstance();
}
