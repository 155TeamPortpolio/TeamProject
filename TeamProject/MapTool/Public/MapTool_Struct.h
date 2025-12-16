#pragma once
#include "Engine_Defines.h"
#include "MapTool_Defines.h"

namespace MapTool {

	typedef struct tagResourceModelPathPack {
		_bool		isLoaded = { false };
		string		TagName = {};
		string		TagModelKey = {};
		string		TagModelPath = {};
		string		TagMaterialKey = {};
		string		TagMaterialPath = {};
	}ModelPathPack;




}

