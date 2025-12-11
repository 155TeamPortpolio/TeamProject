#ifndef Engine_Define_h__
#define Engine_Define_h__
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <Effect_Inc/d3dx11effect.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#include "DirectXTK_Inc/WICTextureLoader.h"
#include "DirectXTK_Inc/DDSTextureLoader.h"
#include "DirectXTK_Inc/ScreenGrab.h"
#include "DirectXTK_Inc/SpriteFont.h"
#include "DirectXTK_Inc/SpriteBatch.h"
#include "DirectXTK_Inc/VertexTypes.h"
#include "DirectXTK_Inc/PrimitiveBatch.h"
#include "DirectXTK_Inc/Effects.h"
#include <DirectXTK_Inc/CommonStates.h>
#include <DirectXTK_Inc/SimpleMath.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <ctime>
#include <memory>
#include <process.h>
#include <typeindex>
#include <filesystem>
#include <sstream>
#include <any>
#include <iostream>
#include <fstream>
#include <variant>
#include <stack>

#include <commdlg.h>										// GetSaveFileName API를 위해 필요
#pragma comment(lib, "Comdlg32.lib")	// 라이브러리 링크

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <FMOD_Inc/fmod.hpp>

// 기존 new 매크로로 인해 충돌 방지
#pragma push_macro("new")
#undef new
#include "GUI_Inc/imgui.h"
#include "GUI_Inc/ImGuizmo.h"
#include "GUI_Inc/backends/imgui_impl_win32.h"
#include "GUI_Inc/backends/imgui_impl_dx11.h"
#include "NFD_Inc/nfd.h"
#pragma pop_macro("new")

//---JSON -- //
#include "Json_Inc/json.hpp"  
using json = nlohmann::json;

#define UNICODE
#define _UNICODE

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_Layouts.h"
#include "Build_Struct.h"
#include "Data_Packets.h"
using namespace Engine;

#include <windowsx.h>

#pragma warning(disable :4251) //맵 컨테이너 경고 무시

#define _USING_GUI


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

#endif // Engine_Define_h__
