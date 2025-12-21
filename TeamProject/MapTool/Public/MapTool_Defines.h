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

	enum class MAPOBJ_TYPE { PLACED, TRIGGER, ALL,END};
	enum class SLOT_DATA_TYPE { Int, Float, Bool, String, Float3, END };
	static std::string g_TagMapToolLevel = "MapTool_Level";
	static std::string g_TagFileFirstName = "MapData.";
	using MTjson = nlohmann::ordered_json;

}


extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace MapTool;

#include "MapTool_Struct.h"
