#pragma once

#include <windows.h>
#include <d3d11.h>


namespace Client {
	static constexpr unsigned int g_iWinSizeX = 1920;
	static constexpr unsigned int g_iWinSizeY = 1080;
	static constexpr unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace Client;

#include "Engine_Defines.h"
