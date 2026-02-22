#include "pch.h"
#include "ZeroStage_Start.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Zero_Level.h"
#include "StageRouter.h"
#include "ZeroPortal.h"
#include "UIDirector.h"
#include "BattlePlayer.h"
/*PostRenderer*/
#include "PostRenderer.h"
#include "PostProcessCommand.h"
#include "AudioSource.h"

CZeroStage_Start::CZeroStage_Start()
{
	m_eType = StageType::Start;
}

HRESULT CZeroStage_Start::Initialize(CZero_Level* pOwnerLevel)
{
	if (!pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;

	return S_OK;
}

HRESULT CZeroStage_Start::Awake()
{
	return S_OK;
}

void CZeroStage_Start::Update()
{
	float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);

	switch (m_eStageState)
	{
	case Client::CStage::StageState::Entrance:
		m_introFlow.Tick(dt);
		Intro();
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

HRESULT CZeroStage_Start::Enter_Stage(StageContext& context)
{
	Ready_Map("Zero_Level", context.mapKey);
	Reserve_Enemy("Zero_Level");
	m_eStageState = StageState::Entrance;
	m_PlayerHandle = context.hPlayer;
	Active_Player(CStage::PlayerPoint::Typical);
	BaseIntro(context);

	m_pOwnerLevel->Get_ZeroFog()->Use_Fog(false);

	m_pOwnerLevel->Get_ZeroCloud()->Set_BaseCloud(
		{
			_vector3{0.197f, 0.232f, 0.226f},
			_vector3{0.028f, 0.03f, 0.031f},
			_vector3{0.f, 0.f, 0.f},
			0.61f,
			_vector3{0.61f, 0.726f, 0.706f},
			_vector3{0.f, 0.f, 0.f},
			0.44f
		}
	);

	_uint Boss_Process{};
	RuntimeBucket().Int64.TryGet(PersistScope::SaveSlot, "Boss_Process", Boss_Process);

	string BGMTag{};
	if (Boss_Process == 1)
	{
		BGMTag = "Hollow_Zero_1.wav";
	}
	else if (Boss_Process == 2)
	{
		BGMTag = "Hollow_Zero_2.wav";
	}

	m_pOwnerLevel->Get_ZeroBGM()->Slot(BGMTag)
		.Attribute3D(false)
		.Group(SOUND_GROUP::BGM)
		.Loop(true)
		.Volume(0.2f)
		.Play();

	return S_OK;
}

void CZeroStage_Start::Intro()
{
	if (m_introFlow.IsDoneAll())
	{
		Active_Enemy();
		CBattleSystem::GetInstance()->SetActive(true);
		m_eStageState = StageState::None;
		Active_Portal();
		BattleSystem()->GetBattlePlayer()->UnLock_Input();
	}
}

void CZeroStage_Start::Outro()
{
	BaseOutro();
	m_eStageState = StageState::End;
}

void CZeroStage_Start::End()
{
	if (m_outroFlow.IsDoneAll()) {
		auto stageType = m_pOwnerLevel->Get_Router()->GetChoiceType(m_iNextChoice);
		m_pOwnerLevel->Get_Router()->Choose(m_iNextChoice);
		m_pOwnerLevel->ChangeStage(stageType);
	}
}

void CZeroStage_Start::Active_Portal()
{
	auto pRouter = m_pOwnerLevel->Get_Router();
	const int choiceCount = pRouter->GetChoiceCount();
	if (choiceCount <= 0) return;
	if (m_pPortals.empty()) return;

	for (size_t i = 0; i < 2; i++)
	{
		if (!m_pPortals[i]) continue;

		auto* zeroPortal = dynamic_cast<CZeroPortal*>(m_pPortals[i]);
		if (zeroPortal) {
			zeroPortal->Set_Alive(true);
			zeroPortal->SetChoiceIndex(this, i);
			zeroPortal->Get_Component<CCollider>()->Set_CompActive(true);
		}
	}
}

CZeroStage_Start* CZeroStage_Start::Create(CZero_Level* pOwnerLevel)
{
	CZeroStage_Start* pInstance = new CZeroStage_Start;
	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CZeroStage_Start::Free()
{
	__super::Free();
}