#pragma once
#include <windows.h>
#include <d3d11.h>


namespace ModelEdit {
	inline unsigned int g_iWinSizeX = 2560;
	inline unsigned int g_iWinSizeY = 1360;
	inline unsigned int g_iMainFrame = 144;
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
