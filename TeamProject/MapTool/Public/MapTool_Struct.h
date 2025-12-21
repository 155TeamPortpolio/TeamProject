#pragma once
#include "MapTool_Defines.h"
#include "Json_Inc/json.hpp"

namespace MapTool {

	typedef struct tagResourceModelPathPack {
		_bool			isLoaded = { false };
		std::string		TagName = {};
		std::string		TagModelKey = {};
		std::string		TagModelPath = {};
		std::string		TagMaterialKey = {};
		std::string		TagMaterialPath = {};
	}ModelPathPack;

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
		_int		iVersion = 1;
		std::vector<MapData_Layer> Layers;
	}MapData_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Header, TagDataFormat, iVersion, Layers);

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
		std::string		id = {};
		std::string		TagName = {};
		SLOT_DATA_TYPE	eDataType = SLOT_DATA_TYPE::END;
		SlotValue		defaultvalue;
		_int			iObjID = { -1 };

		_bool			hasRange = { false };
		_double			minV = { 0.0 };
		_double			maxV = { 1.0 };
	}FIELD_DATA_DEFINE;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FIELD_DATA_DEFINE, iObjID, TagName, defaultvalue);

	typedef struct tagMapSlotDataHeader {
		std::string	TagDataFormat = {};
		_int		iVersion = 1;
		std::vector<FIELD_DATA_DEFINE> values; 
	}MapData_Slot_Header;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Slot_Header, TagDataFormat, iVersion, values);

}