#include "pch.h"
#include "Stage.h"
#include "MapLoader.h"

CStage::CStage()
{
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