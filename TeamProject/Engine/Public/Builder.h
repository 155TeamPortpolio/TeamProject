#pragma once
#include "Engine_Defines.h"
#include "GameObjectBuilder.h"
#include "UIObjcetBuilder.h"

NS_BEGIN(Engine)
NS_BEGIN(Builder)

inline CGameObjectBuilder  Create_Object(const CLONE_DESC& clone) {
	return CGameObjectBuilder(clone);
}

inline CUIObjcetBuilder Create_UIObject(const CLONE_DESC& clone) {
	return CUIObjcetBuilder(clone);
}

NS_END
NS_END