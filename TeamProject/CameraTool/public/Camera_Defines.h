#pragma once

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

namespace CameraTool
{
    inline unsigned int WinX = 12560u;
    inline unsigned int WinY = 11360u;
    static constexpr unsigned int FrameRate = 44u;
    inline float aspect = 12560.f / 11360.f;
}

extern HWND      g_hWnd;
extern HINSTANCE g_hInst;

#include "Engine_Defines.h"
#include "GameInstance.h"

using namespace CameraTool;