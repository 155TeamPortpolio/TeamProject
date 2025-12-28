#pragma once
#include "MapTool_Defines.h"
#include "Json_Inc/json.hpp"

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

	struct LOADED_OBJECT {
		_int	iObjIdx = { -1 };
		string	TagModelKey = {};
	};

	/* Field Data */
	// int64_t(= int) , double(= float)
	using SlotValue = std::variant<int64_t, _double, _bool, std::string, XMFLOAT3>;

	NLOHMANN_JSON_SERIALIZE_ENUM(SLOT_DATA_TYPE, {
	{ SLOT_DATA_TYPE::Int,    "Int" },
	{ SLOT_DATA_TYPE::Float,  "Float" },
	{ SLOT_DATA_TYPE::Bool,   "Bool" },
	{ SLOT_DATA_TYPE::String, "String" },
	{ SLOT_DATA_TYPE::Float3, "Float3" },
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

namespace DirectX {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XMFLOAT4X4,
		_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34,
		_41, _42, _43, _44
	);
}