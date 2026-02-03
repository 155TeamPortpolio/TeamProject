#pragma once
#include "Engine_Defines.h"
#include "Json_Inc/json.hpp"

namespace Client {
	enum class SLOT_DATA_TYPE { Int, Float, Bool, String, Float2, Float3, Float4, END };
	struct SlotValue {
		SLOT_DATA_TYPE type = SLOT_DATA_TYPE::END;
		variant<monostate, _int, _float, _bool, string, _float2, _float3, _float4> value;
	};

	static constexpr const char* MAP_DATA_TAGS[] = {
		"MapData",
		"EntityData",
		"BattleData",
		"LightData"
	};

	/* Map Data */
	#pragma region MapData
	typedef struct tagMapObjectData {
		_int		iObjID = { -1 };
		std::string TagModelResourceKey = {};
		std::string TagMaterialResourceKey = {};
		std::array<_float, 4> vRight = { 1.f, 0.f, 0.f, 0.f };
		std::array<_float, 4> vUp = { 0.f, 1.f, 0.f, 0.f };
		std::array<_float, 4> vLook = { 0.f, 0.f, 1.f, 0.f };
		std::array<_float, 4> vPos = { 0.f, 0.f, 0.f, 1.f };
	}MapData_Object;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Object, iObjID, TagModelResourceKey, TagMaterialResourceKey, vRight, vUp, vLook, vPos);

	typedef struct tagMapDataDefaultDesc {
		std::string TagLayer = {};
		std::vector<MapData_Object> Objects;
	}MapData_Layer;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Layer, TagLayer, Objects);

	typedef struct tagMapDataHeader {
		std::string	TagDataFormat = {};
		_int		iVersion = -1;
		std::vector<MapData_Layer> Layers;
	}MapData_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Header, TagDataFormat, iVersion, Layers);

	typedef struct tagFieldDataDef {
 		_int			iObjID = { -1 };
		std::string		TagName = {};
		SlotValue		defaultvalue; // { "type" : "value" }형식으로 받음
	}FIELD_DATA;
	//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FIELD_DATA, iObjID, TagName, defaultvalue);

	typedef struct tagMapSlotDataHeader {
		std::string	TagDataFormat = {};
		_int		iVersion = -1;
		vector<FIELD_DATA> values;
	}MapData_Slot_Header;
	//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Slot_Header, TagDataFormat, iVersion, values);
#pragma endregion

	/* Entity Data */
	#pragma region EntityData
	typedef struct tagEntityInitDesc
	{
		_int		iEntityID = { -1 };
		string		tagName = {};
		_int		iType = {};
		array<_float, 3> vScale = { 0.f, 0.f, 0.f };
		array<_float, 3> vRotation = { 0.f, 0.f, 0.f };
		array<_float, 3> vTranslation = { 0.f, 0.f, 0.f };
		array<_float, 3> vColSize = { 0.f, 0.f, 0.f };
	}ENTITY_INIT;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ENTITY_INIT, iEntityID, tagName, iType, vScale, vRotation, vTranslation, vColSize);

	typedef struct tagEntityHeader {
		string		TagDataFormat = {};
		string		TagArea = {};
		_int		iVersion = 1;
		vector<ENTITY_INIT> Entities;
	}Entity_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Entity_Header, TagDataFormat, TagArea, iVersion, Entities);
#pragma endregion

	/* BattleData */
	#pragma region BattleData
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
		BATTLE_POINT_DATA					PlayerSpawnPoint = {};
		vector<BATTLE_POINT_SPAWNER_DATA>	Spawners;
		vector<BATTLE_POINT_DATA>			Monsters;
		vector<BATTLE_POINT_DATA>			EndPoints;
	}BATTLE_FIELD_DATA;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BATTLE_FIELD_DATA, TagDataFormat, TagArea, PlayerSpawnPoint, Spawners, Monsters, EndPoints);
#pragma endregion

	/* Light Data */
	#pragma region LightData
	typedef struct tagLightJsonDesc {
		_float4 vOffsetPosition = {};
		_float4	vLightDiffuse = {};
		_float4	vLightAmbient = {};
		_float4	vLightSpecular = {};
		_float	fLightRange = {};
		_float	fLightIntensity = {};
	}LIGHT_DESC_JSON;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LIGHT_DESC_JSON, vOffsetPosition, vLightDiffuse, vLightAmbient, vLightSpecular, fLightRange, fLightIntensity);

	typedef struct tagMapLight
	{
		_int			 iIndex{};
		string			 TagArea = {};
		LIGHT_DESC_JSON	 LightDesc;
		array<_float, 3> vTranslation = { 0.f, 0.f, 0.f };
	}MAP_LIGHT;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MAP_LIGHT, iIndex, LightDesc, vTranslation);

	typedef struct tagLightHeader {
		string		TagDataFormat = {};
		string		TagArea = {};
		_int		iVersion = 1;
		vector<MAP_LIGHT> Lights;
	}Light_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Light_Header, TagDataFormat, TagArea, iVersion, Lights);
#pragma endregion 

	/* Runtime CacheData */
	enum class MAPOBJ_TYPE { PLACED, TRIGGER, INVWALL, ENTITY, BATTLE, LIGHT, END };
	using MapSlotValue = variant<monostate, _int, _float, _bool, string, _float2, _float3, _float4>;

	typedef struct tagCachedMapObject {
		_int			DataIndex;
		string			DataName;
		OBJECT_HANDLE	Handle;
		unordered_map<string, MapSlotValue> SlotValues;

		template<typename T>
		T Get_SlotValue(const string& SlotTag) const
		{
			auto iter = SlotValues.find(SlotTag);
			if (iter == SlotValues.end())
				return nullptr;

			if (auto Data = std::get_if<T>(&iter->second))
				return *Data;
			
			return T{};
		}
	}CACHED_OBJECT;

	typedef struct tagCachedBattleData {
		_bool						HasBattleData = { false };
		vector<BATTLE_POINT_DATA>	PlayerPoint;
		vector<BATTLE_POINT_DATA>	MonsterPoint;
		vector<BATTLE_POINT_DATA>	PortalPoint;
		vector<BATTLE_POINT_DATA>	Spawner;
	}CACHED_BATTLE_DATA;

	typedef struct tagCashedData {
		string MapDataTag;
		vector<CACHED_OBJECT> MapObj;
		vector<CACHED_OBJECT> Trigger;
		vector<CACHED_OBJECT> InvWall;
		vector<CACHED_OBJECT> Entity;
		vector<CACHED_OBJECT> Light;

		CACHED_BATTLE_DATA	  Battle;

		const vector<CACHED_OBJECT>* Select(MAPOBJ_TYPE eType) const
		{
			switch (eType)
			{
			case MAPOBJ_TYPE::PLACED:	return &MapObj;
			case MAPOBJ_TYPE::TRIGGER:	return &Trigger;
			case MAPOBJ_TYPE::INVWALL:	return &InvWall;
			case MAPOBJ_TYPE::ENTITY:	return &Entity;
			default:					return nullptr;
			}
		}

		const CACHED_OBJECT* GetDataByVectorIndex(_uint i, MAPOBJ_TYPE eType) const
		{
			const vector<CACHED_OBJECT>* Data = Select(eType);
			if (!Data) return nullptr;
			
			if (!Data || Data->size() <= i) return nullptr;
			return &(*Data)[i];
		}

		const CACHED_OBJECT* GetDataByDataIndex(_uint i, MAPOBJ_TYPE eType) const
		{
			const vector<CACHED_OBJECT>* Data = Select(eType);
			if (!Data) return nullptr;
			
			for (const auto& iter : *Data)
				if (iter.DataIndex == i) return &iter;

			return nullptr;
		}

		const CACHED_OBJECT* GetDataByDataName(const string& NameTag, MAPOBJ_TYPE eType) const
		{
			const vector<CACHED_OBJECT>* Data = Select(eType);
			if (!Data) return nullptr;

			for (const auto& iter : *Data)
				if (iter.DataName == NameTag) return &iter;

			return nullptr;
		}
	}CASHED_OBJ_DATA;
}