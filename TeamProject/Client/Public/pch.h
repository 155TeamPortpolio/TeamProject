#pragma once

#include <windows.h>
#include <d3d11.h>


namespace Client {
	static constexpr unsigned int g_iWinSizeX = 1600;
	static constexpr unsigned int g_iWinSizeY = 900;
	//static constexpr unsigned int g_iWinSizeX = 2560;
	//static constexpr unsigned int g_iWinSizeY = 1440;
	static constexpr unsigned int g_iMainFrame = 144;
	static int	g_iMapDataVersion = { 1 };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace Client;

#include "Engine_Defines.h"

#include "Client_Struct.h"
#include "Client_Enum.h"
#include "Event_Struct.h"