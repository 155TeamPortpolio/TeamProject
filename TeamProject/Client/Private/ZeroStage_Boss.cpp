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
	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageState = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;

	Active_Player(CStage::PlayerPoint::Typical);
	BossIntro(context);

	if ("Zero_Boss1" == context.mapKey) 
	{
		m_pOwnerLevel->Get_ZeroBGM()->Slot("Sacrifice_BGM.wav").Attribute3D(false).Loop(-1).Volume(0.2f).Play();

		m_pOwnerLevel->Get_ZeroFog()->Set_BaseFog(
			{
				_float4{ 0.08f, 0.02f, 0.02f, 1.0f },
				0.02f
			});

		m_pOwnerLevel->Get_ZeroCloud()->Use_Cloud(false);
	}
	else if ("Zero_Boss2" == context.mapKey)
	{
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
		//m_pOwnerLevel->Get_ZeroShadow().
		m_pOwnerLevel->Get_ZeroFog()->Set_BaseFog(
			{
				_float4{0.026f, 0.045f, 0.054f, 1.0f},
				0.00319f
			});
	}

	return S_OK;
}

HRESULT CZeroStage_Boss::Exit_Stage(StageContext& context)
{
	__super::Exit_Stage(context);

	if ("Zero_Boss1" == context.mapKey)
		m_pOwnerLevel->Get_ZeroBGM()->FadeOut_Volume("Sacrifice_BGM.wav", 0.9f);
	
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