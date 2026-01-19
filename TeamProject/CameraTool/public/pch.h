#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN           

#include <windows.h>

#include "Camera_Defines.h"
#include "CameraTool_Enum.h"
#include "CameraTool_Struct.h"

#include "Camera.h"
#include "Light.h"
#include "CamObj.h"
#include "CameraMgr.h"
#include "InputMgr.h"
#include "IInputService.h"
#include "PrototypeMgr.h"
#include "GameInstance.h"

#include "Helper_Func.h"
#include "Engine_Math.h"

#define GAME   CGameInstance::GetInstance()
#define CAM    CGameInstance::GetInstance()->Get_CameraMgr()
#define OBJ    CGameInstance::GetInstance()->Get_ObjectMgr()
#define KEY    CGameInstance::GetInstance()->Get_InputDev()
#define PROTO  CGameInstance::GetInstance()->Get_PrototypeMgr()
#define GUI    CGameInstance::GetInstance()->Get_GUISystem()
#define RES    CGameInstance::GetInstance()->Get_ResourceMgr()
#define DEVICE CGameInstance::GetInstance()->Get_Device()
#define DC     CGameInstance::GetInstance()->Get_Context()