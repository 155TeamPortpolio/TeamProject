#pragma once

#include <windows.h>
#include <d3d11.h>

namespace UITool {
	static constexpr unsigned int g_iWinSizeX = 1920;
	static constexpr unsigned int g_iWinSizeY = 1080;
	static constexpr unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace UITool;

#include "Engine_Defines.h"
#include "Engine_Math.h"

// 엔진에 넣어야 ??
typedef struct tagButtonEvent {
	wstring msg = L"";
}BTN_EVENT;