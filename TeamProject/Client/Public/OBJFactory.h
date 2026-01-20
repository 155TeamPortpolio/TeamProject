#pragma once
#include "Engine_Defines.h"
#include "MapTriggerObject.h"

NS_BEGIN(Client)
NS_BEGIN(Factory)

typedef struct tagOBJFactoryDesc {
	string ClassTag;
	_vector3 vPos;
	_vector3 vSize;
	_vector3 vRot;
}FACTORY_DESC;

void Create_Interactable(const CMapTriggerObject::MAP_TRIGGEROBJ_DESC* Desc);
void Create_NPC(const CMapTriggerObject::MAP_TRIGGEROBJ_DESC* Desc);

NS_END

NS_END