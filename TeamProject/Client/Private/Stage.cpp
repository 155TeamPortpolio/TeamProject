#include "pch.h"
#include "Stage.h"
#include "MapLoader.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "UIDirector.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

CStage::CStage()
{
}

void CStage::StageChangeOn(CZero_Level::StageType nextStageType, _int StageID)
{
	m_eStageStage = StageState::Outro;
}

void CStage::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");
	Safe_Release(pMapLoader);
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