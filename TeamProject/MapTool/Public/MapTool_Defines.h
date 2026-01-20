#pragma once

#include <windows.h>
#include <d3d11.h>
#include "Engine_Defines.h"

namespace MapTool {
	//static constexpr unsigned int g_iWinSizeX = 1280;
	//static constexpr unsigned int g_iWinSizeY = 720;
	static constexpr unsigned int g_iWinSizeX = 1600;
	static constexpr unsigned int g_iWinSizeY = 900;
	static constexpr unsigned int g_iMainFrame = 144;
	static const _char* g_tagMapObjType[6] = { 
		"", "PlacedObject_Layer", "TriggerObject_Layer","EntityData_Layer", "BattleData_Layer", "All_Layer"};
	enum class MAPOBJ_TYPE { NONE, PLACED, TRIGGER, ENTITY, BATTLE, ALL, END};
	enum class SLOT_DATA_TYPE { Int, Float, Bool, String, Float2, Float3, Float4, END };
	static std::string g_TagMapToolLevel = "MapTool_Level";
	static std::string g_TagFileName_MapData = "MapData";
	using MTjson = nlohmann::ordered_json;

}


extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace MapTool;

#include "MapTool_Struct.h"
