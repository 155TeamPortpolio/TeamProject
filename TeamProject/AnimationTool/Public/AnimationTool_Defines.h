#pragma once

#include <windows.h>
#include <d3d11.h>


namespace AnimTool {
	static constexpr unsigned int g_iWinSizeX = 1280;
	static constexpr unsigned int g_iWinSizeY = 720;
	static constexpr unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace AnimTool;

#include "Engine_Defines.h"