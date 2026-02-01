#pragma once
#include "MapTool_Defines.h"
#include "Engine_Json.h"

namespace MapTool {

	typedef struct tagResourceModelPathPack {
		_bool			isLoaded = { false };
		string			TagName = {};
		string			TagModelKey = {};
		string			TagModelPath = {};
		string			TagMaterialKey = {};
		string			TagMaterialPath = {};
	}ModelPathPack;

	typedef struct tagMapObjectData {
		_int		iObjID = { -1 };
		string TagModelResourceKey = {};
		string TagMaterialResourceKey = {};
		array<_float, 4> vRight = { 1.f, 0.f, 0.f, 0.f };
		array<_float, 4> vUp = { 0.f, 1.f, 0.f, 0.f };
		array<_float, 4> vLook = { 0.f, 0.f, 1.f, 0.f };
		array<_float, 4> vPos = { 0.f, 0.f, 0.f, 1.f };
	}MapData_Object;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Object, iObjID, TagModelResourceKey, TagMaterialResourceKey, vRight, vUp, vLook, vPos);

	typedef struct tagMapDataDefaultDesc {
		string TagLayer = {};
		vector<MapData_Object> Objects;
	}MapData_Layer;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Layer, TagLayer, Objects);

	typedef struct tagMapDataHeader {
		string		TagDataFormat = {};
		string		TagArea = {};
		_int		iVersion = 1;
		vector<MapData_Layer> Layers;
	}MapData_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Header, TagDataFormat, TagArea, iVersion, Layers);

	/* Entity Data */
	typedef struct tagEntity
	{
		_int		iEntityID = { -1 };
		string		tagName = {};
		_int		iType = {};
		array<_float, 3> vScale = { 0.f, 0.f, 0.f };
		array<_float, 3> vRotation = { 0.f, 0.f, 0.f };
		array<_float, 3> vTranslation= { 0.f, 0.f, 0.f };
		array<_float, 3> vColSize = { 0.f, 0.f, 0.f };
	}ENTITY;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ENTITY, iEntityID, tagName, iType, vScale, vRotation, vTranslation, vColSize);

	typedef struct tagEntityHeader {
		string		TagDataFormat = {};
		string		TagArea = {};
		_int		iVersion = 1;
		vector<ENTITY> Entities;
	}Entity_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Entity_Header, TagDataFormat, TagArea, iVersion, Entities);


	/* Light Data */
	typedef struct tagLightJsonDesc {
		_float4 vOffsetPosition	= {};
		_float4	vLightDiffuse	= {};
		_float4	vLightAmbient	= {};
		_float4	vLightSpecular	= {};
		_float	fLightRange		= {};
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

	//====================================
	struct LOADED_OBJECT {
		_int	iObjIdx = { -1 };
		string	TagModelKey = {};
	};
	
	struct LOADED_DATA
	{
		string tagDataFormat = {};
		vector<LOADED_OBJECT>	LoadedObjects;
	};

	/* Field Data */
	// int64_t(= int) , double(= float)
	using SlotValue = std::variant<int64_t, _double, _bool, string, XMFLOAT2, XMFLOAT3, XMFLOAT4>;

	NLOHMANN_JSON_SERIALIZE_ENUM(SLOT_DATA_TYPE, {
	{ SLOT_DATA_TYPE::Int,    "Int" },
	{ SLOT_DATA_TYPE::Float,  "Float" },
	{ SLOT_DATA_TYPE::Bool,   "Bool" },
	{ SLOT_DATA_TYPE::String, "String" },
	{ SLOT_DATA_TYPE::Float2, "Float2" },
	{ SLOT_DATA_TYPE::Float3, "Float3" },
	{ SLOT_DATA_TYPE::Float4, "Float4" },
	{ SLOT_DATA_TYPE::END,    "END" }
		});


	typedef struct tagFieldDataDef {
		string			id = {};
		string			TagName = {};
		SLOT_DATA_TYPE	eDataType = SLOT_DATA_TYPE::END;
		SlotValue		defaultvalue;
		_int			iObjID = { -1 };

		_bool			hasRange = { false };
		_double			minV = { 0.0 };
		_double			maxV = { 1.0 };
	}FIELD_DATA_DEFINE;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FIELD_DATA_DEFINE, iObjID, TagName, defaultvalue);

	typedef struct tagMapSlotDataHeader {
		string		TagDataFormat = {};
		string		TagArea = {};
		_int		iVersion = 1;
		vector<FIELD_DATA_DEFINE> values;
	}MapData_Slot_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Slot_Header, TagDataFormat, TagArea, iVersion, values);

	typedef struct tagBoneInfo {
		_int        Index = { -1 };
		string      TagBone = {};
		XMFLOAT4X4   BoneOffsetMatrix = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f };
	}BONE_INFO;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BONE_INFO, Index, TagBone, BoneOffsetMatrix);

	typedef struct tagBoneDataHeader {
		string		TagDataFormat = {};
		string      TagModel = {};
		vector<BONE_INFO>   BoneInfos;
	}BONE_DATA_HEADER;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BONE_DATA_HEADER, TagDataFormat, TagModel, BoneInfos);

}