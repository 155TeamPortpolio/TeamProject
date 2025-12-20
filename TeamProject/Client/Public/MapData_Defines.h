#pragma once
#include "Engine_Defines.h"
#include "Json_Inc/json.hpp"

namespace Client {
	/* Map Data */
	enum class SLOT_DATA_TYPE { NONE, Int, Float, Bool, String, Float3 };

	struct SlotValue {
		SLOT_DATA_TYPE type = SLOT_DATA_TYPE::NONE;
		variant<monostate, int64_t, double, bool, string, XMFLOAT3> value;
	};

	typedef struct tagMapObjectData {
		std::string TagModelResourceKey = {};
		std::string TagMaterialResourceKey = {};
		std::array<_float, 4> vRight = { 1.f, 0.f, 0.f, 0.f };
		std::array<_float, 4> vUp = { 0.f, 1.f, 0.f, 0.f };
		std::array<_float, 4> vLook = { 0.f, 0.f, 1.f, 0.f };
		std::array<_float, 4> vPos = { 0.f, 0.f, 0.f, 1.f };
	}MapData_Object;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Object, TagModelResourceKey, TagMaterialResourceKey, vRight, vUp, vLook, vPos);

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

	typedef struct tagSlotDataParsePerObj {

	};

}