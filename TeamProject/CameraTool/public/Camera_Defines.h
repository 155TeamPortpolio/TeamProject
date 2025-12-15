#pragma once

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

namespace CameraTool
{
	//static constexpr unsigned int WinX    = 1280;
	//static constexpr unsigned int WinY    = 720;
	static constexpr unsigned int WinX      = 2560;
	static constexpr unsigned int WinY      = 1360;
	static constexpr unsigned int FrameRate = 144;
}

extern HWND      g_hWnd;
extern HINSTANCE g_hInst;

#include "Engine_Defines.h"
#include "GameInstance.h"

using namespace CameraTool;