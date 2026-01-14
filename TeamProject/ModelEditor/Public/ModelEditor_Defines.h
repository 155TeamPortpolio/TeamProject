#pragma once
#include <windows.h>
#include <d3d11.h>


namespace ModelEdit {
	//static constexpr unsigned int g_iWinSizeX = 1280;
	//static constexpr unsigned int g_iWinSizeY = 720;
	static constexpr unsigned int g_iWinSizeX = 1600;
	static constexpr unsigned int g_iWinSizeY = 900;
	//static constexpr unsigned int g_iWinSizeX = 2560;
	//static constexpr unsigned int g_iWinSizeY = 1360;
	static constexpr unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance; 


using namespace ModelEdit;

#include "Engine_Defines.h"   // 엔진 디버그 new 세팅이 여기서 완료된 상태

#pragma push_macro("new")
#undef new

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#pragma pop_macro("new")
