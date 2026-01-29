#pragma once

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

namespace CameraTool
{
    inline unsigned int WinX = 12560u;
    inline unsigned int WinY = 11360u;
    static constexpr unsigned int FrameRate = 144u;
    inline float aspect = 22560.f / 13260.f;
}

extern HWND      g_hWnd;
extern HINSTANCE g_hInst;

#include "Engine_Defines.h"
#include "GameInstance.h"

using namespace CameraTool;