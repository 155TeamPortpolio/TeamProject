#pragma once

#include <windows.h>
#include <d3d11.h>


namespace MapTool {
	static constexpr unsigned int g_iWinSizeX = 1280;
	static constexpr unsigned int g_iWinSizeY = 720;
	static constexpr unsigned int g_iMainFrame = 144;

	enum class ObjectType { Static, Floor, Trigger, Navigation, END};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace MapTool;

#include "Engine_Defines.h"
#include "MapTool_Struct.h"
