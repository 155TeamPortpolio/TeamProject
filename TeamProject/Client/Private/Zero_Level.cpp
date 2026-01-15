#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "BattleSystem.h"

// Camera
#include "Camera.h"
#include "FreeCam.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "ShadowCam.h"
#include "SequenceCam.h"
#include "CamPanel.h"
#include "CamLoader.h"

#include "Player.h"

/* Enemy */
#include "Sacrifice.h" 
#include "SacrificeHand.h"
#include "Sacrifice_Laser.h"
#include "Sacrifice_Orb.h"


#include "MapPlacedObject.h"
#include "MapTriggerObject.h"

CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{CCamDirector::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pCamDirector);
}

HRESULT CZero_Level::Initialize()
{
	CBattleSystem::GetInstance()->SetActive(true);
	RenderSystem()->Set_FogDesc({ _float4(0.12f, 0.25f, 0.35f, 1.0f),0.f, 0.f, 0.005f, true });

	Rake_MapResources();
	auto boss = CZeroStage_Boss::Create(this);
	m_StageContainer.emplace(StageType::Boss, boss);

	m_Context.eStageType = StageType::Boss;
	m_Context.pNowStage = boss;
	m_Context.pNowStage->Ready_Stage(m_Context);

	/* Pre load Ȱ��ȭ ������ ��� ���⼭ ���� */
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
	/* Enemy */
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Sacrifice", CSacrifice::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeHand", CSacrificeHand::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeLaser", CSacrifice_Laser::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeOrb", CSacrifice_Orb::Create());

	/* Player */
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_TestPlayer", CPlayer::Create());
	auto Player = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_TestPlayer" })
		.Build("Test_Player");

	ObjectManager()->Add_Object(Player, { "Zero_Level","Model_Layer"});

	m_pCamDirector->SetSpaceRef(CBattleSystem::GetInstance()->GetCurCharacterHandle());
	//m_pCamDirector->RequestSequence("Jane_Intro", 0.f, true, 0.5f);

	return S_OK;
}

void CZero_Level::Update()
{
	m_Context.pNowStage->Update();

	CBattleSystem::GetInstance()->Update();

	static OBJECT_HANDLE prevPlayer{};

	OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();

	if (curPlayer.isValid() && curPlayer.Get() != prevPlayer.Get())
	{
		prevPlayer = curPlayer;

		m_pCamDirector->SetSpaceRef(curPlayer);

		auto orbitObj = ObjectManager()->Request_Object(m_pCamDirector->GetCamHandle(CamType::Orbit));
		static_cast<COrbitCam*>(orbitObj)->SetTarget(curPlayer);
	}

	if (InputDevice()->Key_Down(VK_F1))
	{
		auto obj = ObjectManager()->Request_Object(m_pCamDirector->GetCamHandle(CamType::Free));
		CameraManager()->Set_MainCam(obj->Get_Component<CCamera>(), 0.5f);
	}

	if (InputDevice()->Key_Down(VK_F2))
	{
		const OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();

		auto obj = ObjectManager()->Request_Object(m_pCamDirector->GetCamHandle(CamType::Orbit));
		static_cast<COrbitCam*>(obj)->SetTarget(curPlayer);

		CameraManager()->Set_MainCam(obj->Get_Component<CCamera>(), 0.5f);
	}

	if (InputDevice()->Key_Down(VK_F3))
		m_pCamDirector->RequestSequence("Jane_Intro", 0.f, true, 0.5f);

	if (InputDevice()->Key_Tap(VK_F7))
		CameraManager()->AddShake(CamShakeType::HitNormal);

	if (InputDevice()->Key_Tap(VK_F8))
		CameraManager()->AddShake(CamShakeType::HitHeavy);

	//	m_pCamDirector->RequestSequence("Jane_Intro_2", 0.f, true, 0.5f);

	m_pCamDirector->Update(m_pGameInstance->Get_EngineDeltaTime());

	if (InputDevice()->Key_Tap(VK_F4))
	{
		CBattleSystem::GetInstance()->SpawnMosnter("Proto_GameObject_Sacrifice", { 0.f, 0.5f,0.f });
	}
}

HRESULT CZero_Level::Render()
{
	return S_OK;
}

void CZero_Level::PreLoad_Level()
{
	/*���⿡ Add ResourcePath �ֱ�*/
	
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

void CZero_Level::Rake_MapResources()
{
	filesystem::path MapDataFolderPath = "../Bin/Resources/MapData/Model/";
	Helper::EnsureDirectoryExist(MapDataFolderPath);


	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	for (const auto& entry : filesystem::recursive_directory_iterator(MapDataFolderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".model")
		{
			filesystem::path ModelPath = entry.path();
			filesystem::path MaterialPath = ModelPath;
			MaterialPath.replace_extension(".mat");


			pRcsMgr->Add_ResourcePath(ModelPath.filename().string(), ModelPath.string());
			pRcsMgr->Add_ResourcePath(MaterialPath.filename().string(), MaterialPath.string());

		}
	}
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
	CBattleSystem::GetInstance()->DestroyInstance();
	m_pGameInstance->DestroyInstance();
	m_pCamDirector->DestroyInstance();
}
