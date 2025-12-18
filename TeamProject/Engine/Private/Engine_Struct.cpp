#include "Engine_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"

_bool Engine::tagObjectHandle::isValid()
{
	CGameObject* pObj = CGameInstance::GetInstance()->Get_ObjectMgr()->Request_Object({ Level,Layer,hObjID });
	if(pObj)
		return true;
	return false;
}
void Engine::tagObjectHandle::Reset()
{
	Level.clear();
	Layer.clear();
	hObjID = 0;
	return;
}

CGameObject* Engine::tagObjectHandle::Get()
{
	CGameObject* pObj = CGameInstance::GetInstance()->Get_ObjectMgr()->Request_Object({ Level,Layer,hObjID });
	return pObj;
}

void Engine::tagObjectHandle::Release()
{

auto mgr = CGameInstance::GetInstance()->Get_ObjectMgr();

CGameObject* pObj = mgr->Request_Object({ Level, Layer, hObjID });
if (!pObj) { Reset(); return; }

mgr->Remove_Object(pObj);
Reset();
}
