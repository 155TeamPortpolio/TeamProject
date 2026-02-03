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
	static const _char* g_tagMapObjType[] = { 
		"NONE", "PlacedObject_Layer", "TriggerObject_Layer","EntityData_Layer", "BattleData_Layer", "Light_Layer", "All_Layer"};
	enum class MAPOBJ_TYPE { NONE, PLACED, TRIGGER, ENTITY, BATTLE, LIGHT, ALL, END};
	static const _char* g_tagBattleObjType[] = {
		"NONE", "Player_Layer", "Spawner_Layer", "Monster_Layer", "EndPoint_Layer"};
	enum class BATTLE_TYPE { NONE, PLAYER, SPAWNER, MONSTER, ENDPOINT, END};
	enum class SLOT_DATA_TYPE { Int, Float, Bool, String, Float2, Float3, Float4, END };
	static std::string g_TagMapToolLevel = "MapTool_Level";
	static std::string g_TagFileName_MapData = "MapData";
	using MTjson = nlohmann::ordered_json;

}


extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace MapTool;

#include "MapTool_Struct.h"
