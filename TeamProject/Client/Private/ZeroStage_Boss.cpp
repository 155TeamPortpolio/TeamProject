#include "pch.h"
#include "ZeroStage_Boss.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Zero_Level.h"
#include "StageRouter.h"
#include "EffectContainer.h"
#include "CamDirector.h"
#include "BattlePlayer.h"

//component
#include "AudioSource.h"
#include "Light.h"

/*PostRenderer*/
#include "PostRenderer.h"
#include "PostProcessCommand.h"
#include <ProceduralSky.h>
#include "Defiler_Control.h"

CZeroStage_Boss::CZeroStage_Boss()
{
	m_eType = StageType::Boss;
}

HRESULT CZeroStage_Boss::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;

	return S_OK;
}

HRESULT CZeroStage_Boss::Awake()
{
	return S_OK;
}

void CZeroStage_Boss::Update()
{
	float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);
	switch (m_eStageState)
	{
	case Client::CStage::StageState::Entrance:
		m_introFlow.Tick(dt);
		Intro();
		break;
	case Client::CStage::StageState::BattleStart:
		BattleStart();
		break;
	case Client::CStage::StageState::Battle:
		Battle();
		break;
	case Client::CStage::StageState::BattleEnd:
		m_ClearFlow.Tick(dt);
		BattleEnd();
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


HRESULT CZeroStage_Boss::Enter_Stage(StageContext& context)
{
	m_pOwnerLevel->Get_ZeroBGM()->FadeOutAll(0.2f);
	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageState = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;

	Active_Player(CStage::PlayerPoint::Typical);
	BossIntro(context);

	if ("Zero_Boss1" == context.mapKey) 
	{
		m_PrevShadowLight = m_pOwnerLevel->Get_ZeroShadow()->pShadowCam->Get_Component<CLight>()->Get_Desc();
		m_pOwnerLevel->Get_ZeroBGM()->Slot("Sacrifice_ENV_Wind.wav").Attribute3D(false).Loop(-1).Volume(0.2f).Play();
		m_pOwnerLevel->Get_ZeroBGM()->Slot("Sacrifice_BGM.wav").Attribute3D(false).Loop(-1).Volume(0.2f).Play();
		m_pOwnerLevel->Get_ZeroCloud()->Set_BaseCloud({
			_float3{0.f, 0.f ,0.f},
			_float3{0.f, 0.f ,0.f},
			_float3{0.f, 0.f ,0.f},
			1.f,
			_float3{0.322f, 0.357f, 0.463f},
			_float3{0.f, 0.f, 0.f},
			0.91 });
		m_pOwnerLevel->Get_ZeroFog()->Use_Fog(true);
		m_pOwnerLevel->Get_ZeroFog()->Set_BaseFog(
			{
				_float4{ 0.031f, 0.005f, 0.011f, 1.0f },
				0.03f
			});

		LIGHT_DESC lightDesc{};
		lightDesc.vLightDirection = _float4(-0.894f, -0.447f, 0.f, 0.f);
		lightDesc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		lightDesc.vLightAmbient = _float4(95.f / 255.f, 95.f / 255.f, 95.f / 255.f, 1.f);
		lightDesc.vLightSpecular = _float4(12.f / 255.f, 12.f / 255.f, 12.f / 255.f, 1.f);
		lightDesc.fLightIntensity = 0.03f;
		m_pOwnerLevel->Get_ZeroShadow()->Set_Light(
			{
				lightDesc
			});

		{
			auto pEnviromentEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
				.Asset("sacrifice_enviroment.json")
				.Build("Enviroment_Particle");

			ObjectManager()->Add_Object(pEnviromentEffect, { "Zero_Level","Effect_Layer" });
		}
	}
	else if ("Zero_Boss2" == context.mapKey)
	{
		m_pOwnerLevel->Get_ZeroBGM()->Slot("Hollow_Zero_2.wav").Stop();
		m_pOwnerLevel->Get_ZeroBGM()->Slot("DefilerStage_ENV.wav").Attribute3D(false).Loop(-1).Volume(0.3f).Play();
		m_pOwnerLevel->Get_ZeroBGM()->Slot("DefilerStage_ENV2.wav").Attribute3D(false).Loop(-1).Volume(0.2f).Play();
		m_pOwnerLevel->Get_ZeroCloud()->Set_BaseCloud({
			_float3{0.f, 0.f ,0.f},
			_float3{0.f, 0.f ,0.f},
			_float3{0.f, 0.f ,0.f},
			1.f,
			_float3{0.322f, 0.357f, 0.463f},
			_float3{0.f, 0.f, 0.f},
			0.91 });
		m_pOwnerLevel->Get_ZeroCloud()->Set_Moon(true);
		m_pOwnerLevel->Get_ZeroShadow()->Set_ShadowPos({ -266.f,117.f,50.f });
		m_pOwnerLevel->Get_ZeroFog()->Set_BaseFog(
			{
				_float4{0.026f, 0.045f, 0.054f, 1.0f},
				0.00319f
			});

		{
			auto pFog = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
				.Asset("defiler_stage_fog.json")
				.Position(_float3(0.f, -3.7f, 0.f))
				.Build("Stage_Fog");

			ObjectManager()->Add_Object(pFog, { "Zero_Level","Effect_Layer" });
		}
	}

	return S_OK;
}

HRESULT CZeroStage_Boss::Exit_Stage(StageContext& context)
{
	__super::Exit_Stage(context);

	if ("Zero_Boss1" == context.mapKey)
	{
		m_pOwnerLevel->Get_ZeroShadow()->Set_Light(m_PrevShadowLight);
		m_pOwnerLevel->Get_ZeroBGM()->Slot("Sacrifice_ENV_Wind.wav").FadeOut(0.2f);
		m_pOwnerLevel->Get_ZeroBGM()->Slot("Sacrifice_BGM.wav").FadeOut(0.2f);

		
	}
	else if ("Zero_Boss2" == context.mapKey)
	{
		m_pOwnerLevel->Get_ZeroBGM()->Slot("DefilerStage_ENV.wav").FadeOut(0.2f);
		m_pOwnerLevel->Get_ZeroBGM()->Slot("DefilerStage_ENV2.wav").FadeOut(0.2f);

		
	}
	//m_pOwnerLevel->Get_ZeroShadow().
	return S_OK;
}

void CZeroStage_Boss::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		BattleSystem()->GetBattlePlayer()->UnLock_Input();
		if (!HasBattleStarter())
			m_eStageState = StageState::BattleStart;
	}
}

void CZeroStage_Boss::BattleStart()
{
	Active_Enemy();
	CBattleSystem::GetInstance()->SetActive(true);
	CamDirector()->EnterBoss();
	m_eStageState = StageState::Battle;
}

void CZeroStage_Boss::Battle()
{
	_bool isBattleEnd = CBattleSystem::GetInstance()->isMonsterCleared();
	if (isBattleEnd) {
 		WipeOutFX();
		m_eStageState = StageState::BattleEnd;
	}
}


void CZeroStage_Boss::BattleEnd()
{
	if (m_ClearFlow.IsDoneAll())
	{
		CBattleSystem::GetInstance()->SetActive(false);
		m_eStageState = StageState::Outro;
	}
}
void CZeroStage_Boss::Outro()
{
	BaseOutro();
	m_outroFlow.Start();
	m_eStageState = StageState::End;
}

void CZeroStage_Boss::End()
{
	if (m_outroFlow.IsDoneAll()) {
		CamDirector()->ExitBoss();
		LevelManager()->Request_ChangeLevel("Scott_Level", true);
	}
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