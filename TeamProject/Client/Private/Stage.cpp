#include "pch.h"
#include "Stage.h"
#include "MapLoader.h"

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

void CStage::Free()
{
	__super::Free();
}