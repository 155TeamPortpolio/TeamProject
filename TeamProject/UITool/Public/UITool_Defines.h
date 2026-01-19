#pragma once

#include <windows.h>
#include <d3d11.h>

#pragma warning(disable: 4251 4244 4267 4819 4005)

namespace UITool
{
	static constexpr unsigned int g_iWinSizeX = 1600;
	static constexpr unsigned int g_iWinSizeY = 900;
	static constexpr unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace UITool;

#include "Engine_Defines.h"
#include "Engine_Math.h"