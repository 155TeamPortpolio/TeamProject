#pragma once
#include "Engine_Json.h"

namespace Client
{
	/* BattleData */
	typedef struct tagBattlePointData
	{
		string		tagType = {};			// Player, Spawner, Monster Point
		_int		iIndex = { -1 };

		array<_float, 3> vScale = { 0.f, 0.f,  0.f };
		array<_float, 3> vRotation = { 0.f, 0.f, 0.f };
		array<_float, 3> vTranslation = { 0.f, 0.f, 0.f };
	}BATTLE_POINT_DATA;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BATTLE_POINT_DATA, tagType, iIndex, vScale, vRotation, vTranslation);

	typedef struct tagBattleSpawnerPointData : public BATTLE_POINT_DATA
	{
		vector<_int>		MonsterIndices;
	}BATTLE_POINT_SPAWNER_DATA;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BATTLE_POINT_SPAWNER_DATA, tagType, iIndex, vScale, vRotation, vTranslation, MonsterIndices);

	typedef struct tagBattleFieldData
	{
		string	TagDataFormat = "";
		string	TagArea = "";
		BATTLE_POINT_DATA			PlayerSpawnPoint = {};
		vector<BATTLE_POINT_SPAWNER_DATA>	Spawners;
		vector<BATTLE_POINT_DATA>	Monsters;
		vector<BATTLE_POINT_DATA>	EndPoints;
	}BATTLE_FIELD_DATA;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BATTLE_FIELD_DATA, TagDataFormat, TagArea, PlayerSpawnPoint, Spawners, Monsters, EndPoints);


}