#include "pch.h"
#include "BackGroundSpawner.h"
#include "DataBase.h"
#include "GameInstance.h"
#include "BackgroundNpc.h"

void CBackGroundSpawner::CreatePedestrian(string AreaTag)
{
	/*auto data = CDataBase::GetInstance()->Get_CashedData(AreaTag);
	auto MovePoints = data->MovePoint;

	for (size_t i = 0; i < MovePoints.size(); i++)
	{
		CBackgroundNpc::BackgroundDesc* desc = new CBackgroundNpc::BackgroundDesc;
		desc->BackgroundCount = 2;
		desc->movePoint = MovePoints[i];
		desc->isPedestrian = true;
		auto pedstrian = 
			Builder::Create_Object({ "MainCity_Level","Proto_GameObject_CBackgroundNpc" })
			.Add_ObjDesc(desc)
			.Build("Back");
		ObjectManager()->Add_Object(pedstrian, { LevelManager()->Get_NowLevelKey(),"NPC_Layer"});
		pedestrinaHandles.push_back(pedstrian->Get_Handle());
	}*/
}
