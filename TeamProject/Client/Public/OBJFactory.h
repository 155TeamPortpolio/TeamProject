#pragma once
#include "Engine_Defines.h"
#include "MapTriggerObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
NS_BEGIN(Factory)

class CGameObject* Create_NPC(CMapTriggerObject::MAP_TRIGGEROBJ_DESC& Desc);

NS_END
NS_END