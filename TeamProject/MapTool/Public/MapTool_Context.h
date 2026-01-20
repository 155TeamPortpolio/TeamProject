#pragma once
#include "Engine_Defines.h"

typedef struct tagGUIContextInterface {
	string			TagArea = {};
	_int			iVersion = { 1 };
	_bool			isAllDebugRender = { true };
}MAPTOOL_CONTEXT;