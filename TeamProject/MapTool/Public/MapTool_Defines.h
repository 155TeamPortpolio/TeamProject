#pragma once

#include <windows.h>
#include <d3d11.h>
#include "Engine_Defines.h"
#include "MapTool_Struct.h"

namespace MapTool {
	//static constexpr unsigned int g_iWinSizeX = 1280;
	//static constexpr unsigned int g_iWinSizeY = 720;
	static constexpr unsigned int g_iWinSizeX = 1600;
	static constexpr unsigned int g_iWinSizeY = 900;
	static constexpr unsigned int g_iMainFrame = 144;

	enum class ObjectType { Placed, Floor, Trigger, Navigation, END};
	static std::string g_TagMapToolLevel = "MapTool_Level";

}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace MapTool;

