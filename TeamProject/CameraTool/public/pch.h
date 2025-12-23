#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN           

#include <windows.h>

#include "Camera_Defines.h"
#include "CameraTool_Enum.h"
#include "CameraTool_Struct.h"

#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "CamObj.h"
#include "CameraMgr.h"

#include "Helper_Func.h"
#include "Engine_Math.h"

#define CAM   CGameInstance::GetInstance()->Get_CameraMgr()
#define GAME  CGameInstance::GetInstance()
#define OBJ   CGameInstance::GetInstance()->Get_ObjectMgr()