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
		std::string TagModelResourceKey = {};
		std::string TagMaterialResourceKey = {};
		std::array<_float, 4> vRight =	{1.f, 0.f, 0.f, 0.f};
		std::array<_float, 4> vUp =		{ 0.f, 1.f, 0.f, 0.f };
		std::array<_float, 4> vLook =	{ 0.f, 0.f, 1.f, 0.f };
		std::array<_float, 4> vPos =	{ 0.f, 0.f, 0.f, 1.f };
	}MapData_Object;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData_Object, TagModelResourceKey, TagMaterialResourceKey, vRight, vUp, vLook, vPos);

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
}
