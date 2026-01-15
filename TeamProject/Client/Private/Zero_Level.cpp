#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "MapPlacedObject.h"
#include "MapTriggerObject.h"

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
	m_Context.pNowStage->Ready_Stage(m_Context);

	/* Pre load 활성화 전까지 잠시 여기서 생성 */
	{
		//==================== Effect =======================

		auto pResource = ResourceManager();

		/* Assets */
		pResource->Add_ResourcePath("test_particle.json", "../Bin/Resources/Effect/Data/test_particle.json");
		pResource->Add_ResourcePath("spawn_smoke.json", "../Bin/Resources/Effect/Data/spawn_smoke.json");
		pResource->Add_ResourcePath("fog.json", "../Bin/Resources/Effect/Data/fog.json");
		pResource->Add_ResourcePath("hit_ground_smoke.json", "../Bin/Resources/Effect/Data/hit_ground_smoke.json");
		pResource->Add_ResourcePath("hit_ground_smoke_strong.json", "../Bin/Resources/Effect/Data/hit_ground_smoke_strong.json");
		pResource->Add_ResourcePath("core.json", "../Bin/Resources/Effect/Data/core.json");
		pResource->Add_ResourcePath("rock_particle.json", "../Bin/Resources/Effect/Data/rock_particle.json");
		pResource->Add_ResourcePath("sacrifice_spark.json", "../Bin/Resources/Effect/Data/sacrifice_spark.json");
		pResource->Add_ResourcePath("sacrifice_hit_ground_flare.json", "../Bin/Resources/Effect/Data/sacrifice_hit_ground_flare.json");
		pResource->Add_ResourcePath("sacrifice_hit_ground_flare_smoke.json", "../Bin/Resources/Effect/Data/sacrifice_hit_ground_flare_smoke.json");
		pResource->Add_ResourcePath("sacrifice_smoke_trail.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail.json");
		pResource->Add_ResourcePath("sacrifice_smoke_trail.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail.json");
		pResource->Add_ResourcePath("sacrifice_smoke_trail_cone.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail_cone.json");
		pResource->Add_ResourcePath("sacrifice_orb.json", "../Bin/Resources/Effect/Data/sacrifice_orb.json");
		pResource->Add_ResourcePath("sacrifice_smoke_slash.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_slash.json");
		pResource->Add_ResourcePath("sacrifice_sword_slash.json", "../Bin/Resources/Effect/Data/sacrifice_sword_slash.json");

		/* Textures */
		pResource->Add_ResourcePath("attack_sign.png", "../Bin/Resources/Effect/Texture/attack_sign.png");
		pResource->Add_ResourcePath("Eff_Particle_044.png", "../Bin/Resources/Effect/Texture/Eff_Particle_044.png");
		pResource->Add_ResourcePath("Eff_Smoke_046_LB_01.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_046_LB_01.png");
		pResource->Add_ResourcePath("Eff_Smoke_218.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_218.png");
		pResource->Add_ResourcePath("Eff_Smoke_006.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_006.png");
		pResource->Add_ResourcePath("rock0.png", "../Bin/Resources/Effect/Texture/rock0.png");
		pResource->Add_ResourcePath("lightning10.png", "../Bin/Resources/Effect/Texture/lightning10.png");
		pResource->Add_ResourcePath("lightning7.png", "../Bin/Resources/Effect/Texture/lightning7.png");
		pResource->Add_ResourcePath("Flare_UU_02.png", "../Bin/Resources/Effect/Texture/Flare_UU_02.png");
		pResource->Add_ResourcePath("Eff_Burn_LYX_28.png", "../Bin/Resources/Effect/Texture/Eff_Burn_LYX_28.png");
		pResource->Add_ResourcePath("Eff_Smoke_259.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_259.png");
		pResource->Add_ResourcePath("Eff_MeleeTrail_078_YZ_05.png", "../Bin/Resources/Effect/Texture/Eff_MeleeTrail_078_YZ_05.png");
		pResource->Add_ResourcePath("Dissolve.png", "../Bin/Resources/Effect/Texture/Dissolve.png");
		pResource->Add_ResourcePath("Eff_Noise_243_YZ_01.png", "../Bin/Resources/Effect/Texture/Eff_Noise_243_YZ_01.png");
		pResource->Add_ResourcePath("Eff_Smoke_113.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_113.png");

		/* Models */
		pResource->Add_ResourcePath("Smoke_Cone2.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Trail/Smoke_Cone2.model");
		pResource->Add_ResourcePath("Smoke_Cone2.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Trail/Smoke_Cone2.mat");
		pResource->Add_ResourcePath("Sacrifice_Orb.model", "../Bin/Resources/Effect/Model/Sacrifice_Orb/Sacrifice_Orb.model");
		pResource->Add_ResourcePath("Sacrifice_Orb.mat", "../Bin/Resources/Effect/Model/Sacrifice_Orb/Sacrifice_Orb.mat");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash5.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash5/Sacrifice_Smoke_Slash5.model");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash5.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash5/Sacrifice_Smoke_Slash5.mat");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash6.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash6/Sacrifice_Smoke_Slash6.model");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash6.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash6/Sacrifice_Smoke_Slash6.mat");
		pResource->Add_ResourcePath("Sacrifice_Sword_Slash2.model", "../Bin/Resources/Effect/Model/Sacrifice_Sword_Slash2/Sacrifice_Sword_Slash2.model");
		pResource->Add_ResourcePath("Sacrifice_Sword_Slash2.mat", "../Bin/Resources/Effect/Model/Sacrifice_Sword_Slash2/Sacrifice_Sword_Slash2.mat");

	}
	
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

void CZero_Level::PreLoad_Level()
{
	/*여기에 Add ResourcePath 넣기*/
	
}

HRESULT CZero_Level::ChangeStage(StageType nextStageType, _int StageID)
{
	if (m_Context.eStageType == nextStageType && m_Context.pNowStage)
		return S_OK;

	if (m_Context.pNowStage)
		m_Context.pNowStage->Exit_Stage(m_Context);

	auto found = m_StageContainer.find(nextStageType);
	if (found == m_StageContainer.end())
		return E_FAIL;

	m_Context.eStageType = nextStageType;
	m_Context.StageID = StageID;
	m_Context.pNowStage = found->second;

	return m_Context.pNowStage->Enter_Stage(m_Context);
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
	m_Context.pNowStage = nullptr;

	for (auto& pair : m_StageContainer)
		Safe_Release(pair.second);
	m_StageContainer.clear();

	__super::Free();
	m_pGameInstance->DestroyInstance();
}
