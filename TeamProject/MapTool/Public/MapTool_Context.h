#pragma once
#include "Engine_Defines.h"

typedef struct tagGUIContextInterface {
	vector<string>	TagLayers = { "PlacedObject_Layer", "TriggerObject_Layer", "All_Layer" };
	string			TagArea = {};
	_int			iVersion = { 1 };
	_bool			isAllDebugRender = { false };
}MAPTOOL_CONTEXT;