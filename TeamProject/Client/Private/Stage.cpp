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

CStage::CStage()
{
}

void CStage::StageChangeOn(StageType nextStageType, _int StageID)
{
	m_eStageStage = StageState::Outro;
}

void CStage::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);

	const CASHED_OBJ_DATA* datas =  CDataBase::GetInstance()->Get_CashedData(AreaTag);
	if (datas->Battle.HasBattleData) {
		ReadyPlayerPoint(datas->Battle.PlayerPoint);
		ReadyMonsterPoint(datas->Battle.MonsterPoint);
		ReadyPortalPoint(datas->Battle.MonsterPoint);
		datas->Battle.Spawner; /*보류*/
		ReadyMonsterData(LevelTag, AreaTag);
	}

	/*아직 저장 안함*/
	auto& Entity = datas->Entity;
	for (auto& data : Entity) /*엔티티*/
	{
		data.DataName;
		data.Handle;
	}

	auto& MapObj = datas->MapObj;
	for (auto& data : MapObj) /*맵 오브젝트*/
	{
		data.DataName;
		data.Handle;
	}

	auto& InvisibleWall = datas->InvWall;
	for (auto& data : InvisibleWall) /*투명 벽 -> 그냥 냅 메쉬*/
	{
		data.DataName;
		data.Handle;
	}

	auto& Trigger = datas->Trigger;
	for (auto& data : Trigger) /*투명 벽 -> 그냥 냅 메쉬*/
	{
		data.DataName;
		data.Handle;
	}
}

void CStage::Reserve_Enemy(const string& LevelTag)
{
	auto& data = m_MonsterData.CreationData;
	m_MonsterData.SpawnPoint;
	_int spawn = {};
	for (size_t i = 0; i < data.size(); i++)
	{
		if (i >= data.size())
			continue;
		for (size_t j = 0; j < data[i].Count; j++)
		{
			CCT_DESC MonsterCCT;
			MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
			MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
			MonsterCCT.bAutoFit = false;
			MonsterCCT.fHeight = data[i].creationInfo.CCT_fHeight;
			MonsterCCT.fRadius = data[i].creationInfo.CCT_fRadius;
			MonsterCCT.vPos = m_MonsterData.SpawnPoint[spawn];
			MonsterCCT.vPos.y += MonsterCCT.fHeight;
			CEnemy::ENEMY_DESC* enemyDesc = new CEnemy::ENEMY_DESC();
			enemyDesc->iMaxHP = data[i].creationInfo.iMaxHP;
			auto pMonster = Builder::Create_Object({ "Zero_Level",data[i].creationInfo.ProtoTag })
				.Add_ObjDesc(enemyDesc)
				.CharacterController(MonsterCCT)
				.FromPool()
				.Build(data[i].creationInfo.DisplayName);

			if (pMonster) {
				m_pMonsters.push_back(pMonster);
				spawn += 1;
			}
		}
	}
}

void CStage::Active_Enemy()
{
	for (auto* pMonster: m_pMonsters)
	{
		if (!pMonster) continue;
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { "Zero_Level", "Enemy_Layer"});
		BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pMonster->Get_Handle());
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
		auto arr = point[i].vTranslation;
		m_PlayerPoint[i] = { arr[0], arr[1], arr[2], arr[3]};
	}

	return S_OK;
}

HRESULT CStage::ReadyPortalPoint(const vector<BATTLE_POINT_DATA>& point)
{
	if (point.empty()) {
		return S_OK;
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
	auto* monsterSpawnMap = CDataBase::GetInstance()->GetMonsterSpawnData(AreaTag, ENUM(m_Context.eStageType));
	if (!monsterSpawnMap)
		return E_FAIL;

	auto iterator = monsterSpawnMap->find(m_Context.StageID);
	if (iterator == monsterSpawnMap->end())
		return E_FAIL;

	const auto& monsterSpawnData = iterator->second;

	for (size_t index = 0; index < monsterSpawnData.size(); ++index)
	{
		auto creation = CDataBase::GetInstance()->GetMonsterDesc(
			monsterSpawnData[index].Colony,
			monsterSpawnData[index].MonsterID);

		_int count = monsterSpawnData[index].Count;
		m_MonsterData.CreationData.push_back({ creation, count });
	}

	return S_OK;
}

void CStage::BaseIntro(CZero_Level::StageContext& context)
{
	if (!m_introFlowBuilt)
	{
		m_introFlowBuilt = true;

		size_t seqId = m_introFlow.BeginSequence();

		if (context.isFirstIn)
		{
			m_introFlow.AddOnce(seqId, [context]() {if (context.isFirstIn)
			{
				BattleSystem()->GetBattlePlayer()->QuestStart();
				CamDirector()->StartBattleIntro(CamSeqType::ZeroIntro);
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
		}
		else {
			m_introFlow.AddWait(seqId, 0.2f);
			m_introFlow.AddOnce(seqId, [this]() {CUIDirector::GetInstance()->FadeIn_Screen(1.f); });
			m_introFlow.AddWait(seqId, 0.2f);
			m_introFlow.AddOnce(seqId, [this]() {RenderSystem()->Apply_RadialBlur(2.f); });
			m_introFlow.AddWait(seqId, 2.0f);
		}

		m_introFlow.EndSequence(seqId);
	}

	m_introFlow.Start();
}
void CStage::BossIntro(CZero_Level::StageContext& context)
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

			});
		m_introFlow.AddWaitUntil(seqId, []()
			{
				//return !CamDirector()->IsPlaying(CamSeqType::ZeroIntro);
				return !CamDirector()->IsPlaying(CamSeqType::BattleIntro);
			});
		m_introFlow.AddOnce(seqId, [context]() {if (context.isFirstIn)
		{
			CUIDirector::GetInstance()->Show_HUD(CUIDirector::HUD::BATTLE);
		}
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
	}

	m_outroFlow.Start();
}

void CStage::Free()
{
	__super::Free();
}