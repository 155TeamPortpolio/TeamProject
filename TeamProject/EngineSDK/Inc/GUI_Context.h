#pragma once
#include "Base.h"

NS_BEGIN(Engine)

typedef struct ENGINE_DLL tagGUIContextInterface {
	class IObjectService* pObjectManager = { nullptr };
	class IUI_Service* pUIManager = { nullptr };
	class ILevelService* pLevelManager = { nullptr };
	class ICameraService* pCameraManager = { nullptr };
	class IInputService* pInputDevice = { nullptr };
	class CLevel* pSelectedLevel = { nullptr };
	class CLayer* pSelectedLayer = { nullptr };
	_bool bLocked = { false };
	class CGameObject* pSelectedObject = { nullptr };
	_float2 viewPort= {};
}GUI_CONTEXT;

NS_END