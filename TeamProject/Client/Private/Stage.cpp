#include "pch.h"
#include "Stage.h"
#include "MapLoader.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "UIDirector.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "DataBase.h"
#include "Enemy.h"
#include "Character.h"
#include "CharacterController.h"
#include "Layer.h"
#include "Zero_Level.h"
#include "StageRouter.h"
#include "ZeroPortal.h"

CStage::CStage()
{
}

HRESULT CStage::Exit_Stage(StageContext& context)
{
	BattleSystem()->ClearBattleStage();

	m_introFlowBuilt = false;
	m_outroFlowBuilt = false;
	m_iNextChoice = { -1 };

	m_MonsterData.Reset();
	while (!m_MonsterQueue.empty())
		m_MonsterQueue.pop();

	for (size_t i = 0; i < m_pPortals.size(); i++)
		ObjectManager()->Remove_Object(m_pPortals[i]);
	m_pPortals.clear();

	for (size_t i = 0; i < m_MapObjects.size(); i++)
		m_MapObjects[i].Delete();

	m_MapObjects.clear();
	return S_OK;
}

void CStage::StageChangeOn(_int choiceIndex)
{
	m_eStageState = StageState::Outro;
	m_iNextChoice = choiceIndex;
}

void CStage::Change_StageState(StageState eState)
{
	m_eStageState = eState;
}

void CStage::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	m_AreaTag = AreaTag;
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader) {
		MSG_BOX("Failed to Load MapData!");
		return;
	}

	Safe_Release(pMapLoader);	

	const CASHED_OBJ_DATA* datas = CDataBase::GetInstance()->Get_CashedData(AreaTag);
	if (datas->Battle.HasBattleData) {
		ReadyPlayerPoint(datas->Battle.PlayerPoint);
		ReadyMonsterPoint(datas->Battle.MonsterPoint);
		ReadyPortalPoint(datas->Battle.PortalPoint);
		datas->Battle.Spawner;
		ReadyMonsterData(LevelTag, AreaTag);
	}
	m_MapObjects.reserve(100);

	auto& Entity = datas->Entity;
	for (auto& data : Entity) 
	{
		data.DataName;
		data.Handle;
		m_MapObjects.push_back(data.Handle);
	}

	auto& MapObj = datas->MapObj;
	for (auto& data : MapObj)
	{
		data.DataName;
		data.Handle;
		m_MapObjects.push_back(data.Handle);
	}

	auto& InvisibleWall = datas->InvWall;
	for (auto& data : InvisibleWall)
	{
		data.DataName;
		data.Handle;
		m_MapObjects.push_back(data.Handle);
	}

	auto& Trigger = datas->Trigger;
	for (auto& data : Trigger)
	{
		data.DataName;
		data.Handle;
		m_MapObjects.push_back(data.Handle);
	}
}

void CStage::Active_Enemy()
{
	if (m_MonsterQueue.empty())
		return;

	for (auto* pMonster : m_MonsterQueue.front())
	{
		if (!pMonster) continue;
		pMonster->Set_Alive(true);
		BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pMonster->Get_Handle());
	}

	m_MonsterQueue.pop();
}

void CStage::Active_Player(PlayerPoint pointType)
{
	if (!m_PlayerHandle.isValid())
		return;

	auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
	auto castedPlayer = dynamic_cast<CPlayer*>(pPlayer);
	m_PlayerHandle = castedPlayer->Get_CurCharacterHandle();

	auto character = m_PlayerHandle.GetAs<CCharacter>();
	if (!character)
		return;

	auto point = m_PlayerPoint[ENUM(pointType)];
	
	character->Get_CCT()->Set_FootPosition(_vector3{ point.pos.x, point.pos.y, point.pos.z });
	character->Get_Component<CTransform>()->Rotate(_vector3(point.rotation));
	CamDirector()->AutoBattle(CamStartDir::Back);
}

void CStage::Active_Portal()
{
	auto pRouter = m_pOwnerLevel->Get_Router();
	const int choiceCount = pRouter->GetChoiceCount();
	if (choiceCount <= 0) return;
	if (m_pPortals.empty()) return;
	
	for (size_t i = 0; i < choiceCount; i++)
	{
		if (!m_pPortals[i]) continue;

		auto* zeroPortal = dynamic_cast<CZeroPortal*>(m_pPortals[i]);
		if (zeroPortal) {
			zeroPortal->Set_Alive(true);
			zeroPortal->SetChoiceIndex(this,i);
		}
	}
}

HRESULT CStage::ReadyPlayerPoint(const vector<BATTLE_POINT_DATA>& point)
{
	if (point.empty()) {
		MSG_BOX("No Player Point : CStage ReadyMap");
		return E_FAIL;
	}

	for (size_t i = 0; i < point.size(); i++)
	{
		auto translation = point[i].vTranslation;
		auto rotation = point[i].vRotation;

		m_PlayerPoint[i].pos= { translation[0], translation[1], translation[2],1.f };
		m_PlayerPoint[i].rotation= { rotation[0], rotation[1], rotation[2] };
	}

	return S_OK;
}
HRESULT CStage::ReadyPortalPoint(const vector<BATTLE_POINT_DATA>& point)
{
	if (point.empty()) {
		return S_OK;
	}

	for (size_t i = 0; i < point.size(); i++)
	{
		auto trans = point[i].vTranslation;
		auto portal = Builder::Create_Object({ "Zero_Level" ,"Proto_GameObject_ZeroPortal" })
			.Position({ trans[0],  trans[1],  trans[2] })
			.Build("zeroPortal#" + to_string(i));
		portal->Set_Alive(false);
		m_pPortals.push_back(portal);

		portal->Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::INTERACTABLE);
		portal->Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER));

		ObjectManager()->Add_Object(m_pPortals[i], { "Zero_Level","InteractableObject_Layer" });
	}

	return S_OK;
}
HRESULT CStage::ReadyMonsterPoint(const vector<BATTLE_POINT_DATA>& point)
{
	if (point.empty()) {
		return S_OK;
	}

	for (size_t i = 0; i < point.size(); i++)
		m_MonsterData.SpawnPoint.push_back(
			{ point[i].vTranslation[0],point[i].vTranslation[1],point[i].vTranslation[2] }
		);

	return S_OK;
}

HRESULT CStage::ReadyMonsterData(const string& LevelTag, const string& AreaTag)
{
	auto* monsterSpawnMap = CDataBase::GetInstance()->GetMonsterSpawnData(AreaTag, ENUM(m_eType));
	if (!monsterSpawnMap)
		return E_FAIL;

	auto iterator = monsterSpawnMap->find(0);
	if (iterator == monsterSpawnMap->end())
		return E_FAIL;

	for (auto& [Encounter, SpawnData] : *monsterSpawnMap)
	{
		for (auto& Desc : SpawnData)
		{
			auto Creation = CDataBase::GetInstance()->GetMonsterDesc(
				Desc.Colony,
				Desc.MonsterID);

			m_MonsterData.CreationData[Encounter]
				.push_back({ Creation, Desc.Count });
		}
	}

	return S_OK;
}

void CStage::Reserve_Enemy(const string& LevelTag)
{
	auto& CreationData = m_MonsterData.CreationData;
	_int spawn{};

	for (auto& [Encounter, SpawnData] : CreationData) {
		
		vector<class CGameObject*> MonsterQueue;

		for (size_t i = 0; i < SpawnData.size(); i++)
		{
			for (size_t j = 0; j < SpawnData[i].Count; j++)
			{
				CCT_DESC MonsterCCT;
				MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
				MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::PLAYER_ATTACK)
					| ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::GROUND) | ENUM(COLLISION_GROUP::NAP);
				MonsterCCT.bAutoFit = false;
				MonsterCCT.fHeight = SpawnData[i].creationInfo.CCT_fHeight;
				MonsterCCT.fRadius = SpawnData[i].creationInfo.CCT_fRadius;
				MonsterCCT.vPos = m_MonsterData.SpawnPoint[spawn];
				MonsterCCT.vPos.y += MonsterCCT.fHeight;
				CEnemy::ENEMY_DESC* enemyDesc = new CEnemy::ENEMY_DESC();
				enemyDesc->iMaxHP = SpawnData[i].creationInfo.iMaxHP;
				auto pMonster = Builder::Create_Object({ "Zero_Level", SpawnData[i].creationInfo.ProtoTag })
					.Add_ObjDesc(enemyDesc)
					.CharacterController(MonsterCCT)
					.Build(SpawnData[i].creationInfo.DisplayName);

				if (pMonster) {
					MonsterQueue.push_back(pMonster);
					dynamic_cast<CEnemy*>(pMonster)->Set_Alive(false);
					CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { "Zero_Level", "Enemy_Layer" });
					spawn++;
				}
			}
		}

		m_MonsterQueue.push(MonsterQueue);
	}
}

void CStage::BaseIntro(StageContext& context)
{
	if (!m_introFlowBuilt)
	{
		m_introFlowBuilt = true;

		size_t seqId = m_introFlow.BeginSequence();

		if (context.isFirstIn)
		{
			m_introFlow.AddOnce(seqId, [context]() {if (context.isFirstIn)
			{
				//BattleSystem()->GetBattlePlayer()->QuestStart();
				CamDirector()->StartBattleIntro(CamSeqType::ZeroIntro);

				UIDirector()->Hide_HUD(CUIDirector::HUD::BATTLE);
				UIDirector()->Show_SceneFrame();
			}
				});
			m_introFlow.AddWaitUntil(seqId, []()
				{
					return !CamDirector()->IsPlaying(CamSeqType::ZeroIntro);
				});
			m_introFlow.AddOnce(seqId, [context]() {if (context.isFirstIn)
			{
				CUIDirector::GetInstance()->Show_HUD(CUIDirector::HUD::BATTLE);
			}
				});
			m_introFlow.AddOnce(seqId, [&context]() {
				if (context.isFirstIn)
					{
						context.isFirstIn = false;
					}
				});
		}
		else {
			m_introFlow.AddWait(seqId, 0.2f);
			m_introFlow.AddOnce(seqId, [this]() {CUIDirector::GetInstance()->FadeIn_Screen(1.f); });
			m_introFlow.AddWait(seqId, 0.2f);
			//m_introFlow.AddOnce(seqId, [this]() {RenderSystem()->Apply_RadialBlur(2.f); });
			m_introFlow.AddWait(seqId, 2.0f);
		}

		m_introFlow.EndSequence(seqId);
	}

	m_introFlow.Start();
}

void CStage::BossIntro(StageContext& context)
{
	if (!m_introFlowBuilt)
	{
		m_introFlowBuilt = true;

		size_t seqId = m_introFlow.BeginSequence();
		m_introFlow.AddWait(seqId, 0.2f);
		m_introFlow.AddOnce(seqId, [this]() {CUIDirector::GetInstance()->FadeIn_Screen(1.f); });
		m_introFlow.AddOnce(seqId, [context]() {
			//BattleSystem()->GetBattlePlayer()->QuestStart();
			
			CamDirector()->StartBattleIntro(CamSeqType::BattleIntro);

			UIDirector()->Hide_HUD(CUIDirector::HUD::BATTLE);
			UIDirector()->Show_SceneFrame();
			});
		m_introFlow.AddWaitUntil(seqId, []()
			{
				//return !CamDirector()->IsPlaying(CamSeqType::ZeroIntro);
				return !CamDirector()->IsPlaying(CamSeqType::BattleIntro);
			});
		m_introFlow.AddOnce(seqId, [context]()
			{
				CUIDirector::GetInstance()->Show_HUD(CUIDirector::HUD::BATTLE);
			});
		m_introFlow.EndSequence(seqId);
	}

	m_introFlow.Start();
}

void CStage::BaseOutro()
{
	if (!m_outroFlowBuilt) {
		m_outroFlowBuilt = true;
		size_t seqId = m_outroFlow.BeginSequence();
		m_outroFlow.AddOnce(seqId, [this]() {CUIDirector::GetInstance()->FadeOut_Screen(1.f); });
		m_outroFlow.AddWait(seqId, 2.0f);
		//m_outroFlow.AddOnce(seqId, [this]() {RenderSystem()->UnRegister_AddictiveColor(); });
	}

	m_outroFlow.Start();
}

_bool CStage::HasBattleStarter()
{
	if (const auto Database = CDataBase::GetInstance()->Get_CashedData(m_AreaTag))
		if (Database->GetDataByDataName("BattleStarter", MAPOBJ_TYPE::ENTITY))
			return true;

	return false;
}

void CStage::Free()
{
	__super::Free();
}