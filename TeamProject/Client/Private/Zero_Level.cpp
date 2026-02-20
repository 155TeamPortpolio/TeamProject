#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "BattleSystem.h"

#include "Helper_Func.h"
#include "EffectContainer.h"
#include "RunTimeBucket.h"

#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "ZeroStage_Elite.h"
#include "ZeroStage_Normal.h"
#include "ZeroStage_Start.h"
#include "TestCloud.h"

// Camera
#include "Camera.h"
#include "CamDirector.h"
#include "UI_MeshPyramid.h"
#include "Player.h"

/* Enemy */
#include "Sacrifice.h" 
#include "SacrificeHand.h"
#include "Sacrifice_Laser.h"
#include "Sacrifice_Orb.h"
#include "Hand_Core.h"
#include "Hand_Sword.h"
#include "ThugBulkyEnforcer.h"
#include "ThugAssaulter.h"
#include "Defiler.h"
#include "MiasmaBlade.h"
#include "DefilerLaser.h"

#include "EnemyAttackCollider.h"
#include "EnemyTriggerCollider.h"
#include "StageRouter.h"
#include "ThugPoacher.h"
#include "ThugPoacher_Arrow.h"
#include "Claymore.h"
#include "Cyclops.h"
#include "Cyclops_Spit.h"
#include "StrikeJaeger.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Shield.h"
#include "Giant.h"

/* UI */
#include "UIDirector.h"

/* Interactable */
#include "ZeroPortal.h"

/*Map*/
#include "MapLoader.h"
#include "DataBase.h"

/*PostRenderer*/
#include "PostRenderer.h"
#include "PostProcessCommand.h"
#include "ProceduralSky.h"

#include "AudioSource.h"

CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey)
{
}

HRESULT CZero_Level::Initialize()
{
	Ready_Prototype();

	/* UI */
	CUIDirector::GetInstance()->Load_LevelObjects("Zero_Level");

	/*ENV*/
	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(true);

	RenderSystem()->GetPostRenderer()->GetCommand<CFogCommand>()
		->SetColor(_float4(0.08f, 0.02f, 0.02f, 1.0f))
		->SetDensity(0.02f)
		->SetEnable(true);
	
	/* BGM */
	m_pBGM = CAudioSource::Create();
	m_pBGM->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Zero/BGM");

	/* Player */
	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	castedPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);
	
	m_Context.isFirstIn = true;
	m_Context.hPlayer = castedPlayer->Get_CurCharacterHandle();
	Ready_Stage();

	{
		auto pEnviromentEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_enviroment.json")
			.Build("Enviroment_Particle");

		ObjectManager()->Add_Object(pEnviromentEffect, { "Zero_Level","Effect_Layer" });
	}
	
	return S_OK;
}

HRESULT CZero_Level::Awake()
{
	if (!m_Context.hPlayer.isValid())
		return E_FAIL;

	return S_OK;
}

void CZero_Level::Update()
{
	CBattleSystem::GetInstance()->Update();
	m_Context.pNowStage->Update();
	BattleSystem()->Update();

	_float dt = GameInstance()->Get_TimeMgr()->Get_DeltaTime(G_EngineTimerID);
	m_tZeroCloud.Update_Cloud(dt);
	m_tZeroFog.Update_Fog(dt);

	if (GetAsyncKeyState('N')) {
		m_tZeroFog.Change_FogState(FOG_DESC{ _float4(1,1,1,1), 1.f }, 1.f, EaseType::Linear);
	}

	if (GetAsyncKeyState('M')) {
		m_tZeroFog.RollBack_Fog(1.f, EaseType::Linear);
	}
}

HRESULT CZero_Level::Render()
{
	return S_OK; 
}

HRESULT CZero_Level::ChangeStage(StageType type)
{
	auto found = m_StageContainer.find(type);
	if (found == m_StageContainer.end())
		return E_FAIL;

	if (m_Context.pNowStage)
		m_Context.pNowStage->Exit_Stage(m_Context);

	m_Context.nextType = type;
	m_Context.mapKey = PopMapKey(type);

	m_Context.pNowStage = found->second;
	return m_Context.pNowStage->Enter_Stage(m_Context);
}

string CZero_Level::PopMapKey(StageType type)
{
	auto it = m_mapCycle.find(type);
	if (it == m_mapCycle.end())
		return {};
	return it->second.Next();
}

void CZero_Level::Ready_Prototype()
{
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_EnemyTriggerCollider", CEnemyTriggerCollider::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Defiler", CDefiler::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaBlade", CMiasmaBlade::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_DefilerLaser", CDefilerLaser::Create());

	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ZeroPortal", CZeroPortal::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_LevelObject_ZeroPortal", CStageRouter::Create());
	
	/* Enemy */
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugBulkyEnforcer", CThugBulkyEnforcer::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugAssaulter", CThugAssaulter::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Defiler", CDefiler::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugPoacher", CThugPoacher::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_ThugPoacher_Arrow", CThugPoacher_Arrow::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Claymore", CClaymore::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Cyclops", CCyclops::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Cyclops_Spit", CCyclops_Spit::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_StrikeJaeger", CStrikeJaeger::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MeleeJaeger", CMeleeJaeger::Create());
	PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_MeleeJaeger_Shield", CMeleeJaeger_Shield::Create());
	PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_Giant", CGiant::Create());

	PrototypeManager()->Add_ProtoType("Zero_Level",	"Proto_GameObject_SacrificeHand", CSacrificeHand::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level",	"Proto_GameObject_SacrificeLaser", CSacrifice_Laser::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_SacrificeOrb", CSacrifice_Orb::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_Sacrifice", CSacrifice::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_HandCore", CHand_Core::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_HandSword", CHand_Sword::Create());
}

void CZero_Level::Ready_Stage()
{
	/*Stage*/
	m_pRouter = Add_LevelObject<CStageRouter>();Safe_AddRef(m_pRouter);
	ObjectManager()->Add_Object(m_pRouter, { "Zero_Level","Router_Layer" });

	m_StageContainer.emplace(StageType::Start, CZeroStage_Start::Create(this));
	m_StageContainer.emplace(StageType::Rest, CZeroStage_Start::Create(this));
	m_StageContainer.emplace(StageType::Normal, CZeroStage_Normal::Create(this));
	m_StageContainer.emplace(StageType::Elite, CZeroStage_Elite::Create(this));
	m_StageContainer.emplace(StageType::Boss, CZeroStage_Boss::Create(this));

	//BuildGraph
	m_pRouter->BuildGraph(2, StageType::Start, StageType::Elite);

	//Start
	m_mapCycle[StageType::Start].maps	= { "Zero_Start1" };
	m_mapCycle[StageType::Rest].maps = { "Zero_Start2" };

	//Normal
 	m_mapCycle[StageType::Normal].maps	= { "Zero_1_1",	"Zero_1_2", "Zero_2_1", "Zero_5_1" };
	Shuffle_MapCycle(m_mapCycle[StageType::Normal].maps);

	//Elite
	m_mapCycle[StageType::Elite].maps	= { "Zero_1_1", "Zero_2_1" };
	Shuffle_MapCycle(m_mapCycle[StageType::Elite].maps);

	//Boss
	_uint Boss_Process{};
	if(!RuntimeBucket().Int64.TryGet(PersistScope::SaveSlot, "Boss_Process", Boss_Process))
		Boss_Process = 2; //Start BossMap Index;

	m_mapCycle[StageType::Boss].maps.push_back("Zero_Boss" + to_string(Boss_Process));
	RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "Boss_Process", (++Boss_Process <=2)? Boss_Process : 1);
	ChangeStage(StageType::Boss);
}

void CZero_Level::Shuffle_MapCycle(vector<string>& Map)
{
	if (Map.size() <= 1)
		return;

	for (size_t i = Map.size() - 1; i > 0 ; --i)
	{
		_int Rand = Helper::Get_Random_Int(0, i);
		swap(Map[i], Map[Rand]);
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
	__super::Free();

	m_Context.pNowStage = nullptr;
	for (auto& pair : m_StageContainer)
		Safe_Release(pair.second);
	m_StageContainer.clear();

	RenderSystem()->GetPostRenderer()->GetCommand<CFogCommand>()->
		SetEnable(false);

	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	castedPlayer->Clear_Characters();
	Safe_Release(m_pBGM);
}

#pragma region Fog
void CZero_Level::Zero_Fog::Update_Fog(_float dt)
{
	if (eUpdate == ZeroUpdate::Nope)
		return;

	fElapsed += dt;

	if (fDuration <= fElapsed) {
		fElapsed = fDuration;
		eUpdate = ZeroUpdate::Nope;
	}

	_float fRatio = (fDuration > 0.f) ? (fElapsed / fDuration) : 1.f;
	_float t = Math::ApplyEase(eEase, clamp(fRatio, 0.f, 1.f));

	const FOG_DESC* pSrcDesc = nullptr;
	const FOG_DESC* pDstDesc = nullptr;

	if (eUpdate == ZeroUpdate::Target) {
		pSrcDesc = &tCurFog;
		pDstDesc = &tTargetFog;
	}
	else if (eUpdate == ZeroUpdate::RollBack) {
		pSrcDesc = &tCurFog;
		pDstDesc = &tBaseFog;
	}

	if (pSrcDesc == nullptr || pDstDesc == nullptr) {
		eUpdate = ZeroUpdate::Nope;
		return;
	}

	Vector4 fogColor = XMVectorLerp(
		XMLoadFloat4(&pSrcDesc->fogColor),
		XMLoadFloat4(&pDstDesc->fogColor),
		t
	);
	_float fogDensity = pSrcDesc->fogDensity + (pDstDesc->fogDensity - pSrcDesc->fogDensity) * t;

	tCurFog = { fogColor, fogDensity };

	RenderSystem()->GetPostRenderer()
		->GetCommand<CFogCommand>()
		->SetFogDesc(tCurFog)
		->SetEnable(true);

}

void CZero_Level::Zero_Fog::Set_BaseFog(FOG_DESC FogDesc)
{
	tCurFog = tBaseFog = FogDesc;

	RenderSystem()->GetPostRenderer()->GetCommand<CFogCommand>()
		->SetFogDesc(tBaseFog);
}

void CZero_Level::Zero_Fog::Change_FogState(FOG_DESC FogDesc, _float fTime, EaseType eEaseType)
{
	eUpdate = ZeroUpdate::Target;
	tTargetFog = FogDesc;
	fElapsed   = 0.f;
	fDuration  = fTime;
	eEase  = eEaseType;
}

void CZero_Level::Zero_Fog::RollBack_Fog(_float fTime, EaseType eEaseType)
{
	eUpdate = ZeroUpdate::RollBack;
	fElapsed = 0.f;
	fDuration = fTime;
	eEase = eEaseType;
	tTargetFog = RenderSystem()->GetPostRenderer()->GetCommand<CFogCommand>()->GetFogDesc();
}

void CZero_Level::Zero_Fog::Use_Fog(_bool b)
{
	RenderSystem()->GetPostRenderer()->GetCommand<CFogCommand>()
		->SetEnable(b);
}
#pragma endregion

#pragma region Cloud
void CZero_Level::Zero_Cloud::Update_Cloud(_float dt)
{
	if (eUpdate == ZeroUpdate::Nope)
		return;

	fElapsed += dt;

	if (fDuration <= fElapsed) {
		fElapsed = fDuration;
		eUpdate = ZeroUpdate::Nope;
	}

	_float fRatio = (fDuration > 0.f) ? (fElapsed / fDuration) : 1.f;
	_float t = Math::ApplyEase(eEase, clamp(fRatio, 0.f, 1.f));

	const CLOUD_DESC* pSrcDesc = nullptr;
	const CLOUD_DESC* pDstDesc = nullptr;

	if (eUpdate == ZeroUpdate::Target) {
		pSrcDesc = &tCurCloud;
		pDstDesc = &tTargetCloud;
	}
	else if (eUpdate == ZeroUpdate::RollBack) {
		pSrcDesc = &tCurCloud;
		pDstDesc = &tBaseCloud;
	}

	if (pSrcDesc == nullptr || pDstDesc == nullptr) {
		eUpdate = ZeroUpdate::Nope;
		return;
	}
		
	Vector3 topCol = XMVectorLerp(
		XMLoadFloat3(&pSrcDesc->topColor),
		XMLoadFloat3(&pDstDesc->topColor), t);
	Vector3 horizonCol = XMVectorLerp(
		XMLoadFloat3(&pSrcDesc->horizonColor),
		XMLoadFloat3(&pDstDesc->horizonColor), t);
	Vector3 hazeCol = XMVectorLerp(
		XMLoadFloat3(&pSrcDesc->hazeColor),
		XMLoadFloat3(&pDstDesc->hazeColor), t);
	_float atmoBlend = pSrcDesc->atmosphereBlend + (pDstDesc->atmosphereBlend - pSrcDesc->atmosphereBlend) * t;
	Vector3 brightCol = XMVectorLerp(
		XMLoadFloat3(&pSrcDesc->cloudBright),
		XMLoadFloat3(&pDstDesc->cloudBright), t);
	Vector3 darkCol = XMVectorLerp(
		XMLoadFloat3(&pSrcDesc->cloudDark),
		XMLoadFloat3(&pDstDesc->cloudDark), t);
	_float coverage = pSrcDesc->coverage + (pDstDesc->coverage - pSrcDesc->coverage) * t;

	tCurCloud = { topCol,horizonCol,hazeCol,atmoBlend,brightCol,darkCol,coverage };
	auto pCloud = dynamic_cast<CProceduralSky*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud)));
	if (pCloud)pCloud->Set_CloudInfo(tCurCloud);
}

void CZero_Level::Zero_Cloud::Set_BaseCloud(CLOUD_DESC CloudDesc)
{
	tCurCloud = tBaseCloud = CloudDesc;

	auto pCloud = dynamic_cast<CProceduralSky*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud)));
	if (pCloud) pCloud->Set_CloudInfo(tBaseCloud);
}

void CZero_Level::Zero_Cloud::Change_CloudState(CLOUD_DESC CloudDesc, _float fTime, EaseType eEaseType)
{
	eUpdate = ZeroUpdate::Target;
	tTargetCloud = CloudDesc;
	fElapsed = 0.f;
	fDuration = fTime;
	eEase = eEaseType;
}

void CZero_Level::Zero_Cloud::RollBack_Cloud(_float fTime, EaseType eEaseType)
{
	eUpdate = ZeroUpdate::RollBack;
	fElapsed = 0.f;
	fDuration = fTime;
	eEase = eEaseType;

	auto pCloud = dynamic_cast<CProceduralSky*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud)));
	if (pCloud)
		tTargetCloud = pCloud->Get_CloudInfo();
}

void CZero_Level::Zero_Cloud::Use_Cloud(_bool b)
{
	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	if(pCloud) pCloud->Set_Alive(b);
}

#pragma endregion