#include "pch.h"
#include "Stage.h"
#include "MapLoader.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "UIDirector.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "DataBase.h"

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
	}

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

HRESULT CStage::ReadyPlayerPoint(const vector<BATTLE_POINT_DATA>& point)
{
	if (point.empty()) {
		MSG_BOX("No Player Point : CStage ReadyMap");
		return E_FAIL;
	}

	for (size_t i = 0; i < point.size(); i++)
	{
		BATTLEOBJ_INFO info = {};
		info.vPos = { point[i].vTranslation[0],point[i].vTranslation[1],point[i].vTranslation[2] };
		m_Context.player.push_back(move(info));
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
		BATTLEOBJ_INFO info = {};
		info.vPos = { point[i].vTranslation[0],point[i].vTranslation[1],point[i].vTranslation[2] };
		m_Context.portal.push_back(move(info));
	}

	return S_OK;
}

HRESULT CStage::ReadyMonsterPoint(const vector<BATTLE_POINT_DATA>& point)
{
	if (point.empty()) {
		return S_OK;
	}

	for (size_t i = 0; i < point.size(); i++)
	{
		BATTLEOBJ_INFO info = {};
		info.vPos = { point[i].vTranslation[0],point[i].vTranslation[1],point[i].vTranslation[2] };
		m_Context.monster.push_back(move(info));
	}
	return S_OK;
}

HRESULT CStage::ReadyMonsterData(const string& LevelTag, const string& AreaTag)
{
	auto* datas = CDataBase::GetInstance()->GetMonsterSpawnData(AreaTag,ENUM(m_Context.eStageType));
	auto iter = *datas->find(m_Context.StageID);
	if(iter == *datas->end())
		return E_FAIL;
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