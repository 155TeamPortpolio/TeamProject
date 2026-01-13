#pragma once

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

namespace CameraTool
{
    inline unsigned int WinX = 2560;
    inline unsigned int WinY = 1360;
    static constexpr unsigned int FrameRate = 144;
    inline float aspect = 2560.f / 1360.f;
}

extern HWND      g_hWnd;
extern HINSTANCE g_hInst;

#include "Engine_Defines.h"
#include "GameInstance.h"

using namespace CameraTool;