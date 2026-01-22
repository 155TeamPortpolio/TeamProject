#pragma once
#include "Engine_Defines.h"
#include "MapTriggerObject.h"

NS_BEGIN(Client)
NS_BEGIN(Factory)

typedef struct tagOBJFactoryDesc {
	_int    iEntityID{};
	string  tagName{};
	string	tagLevel{};
	_int    iType{};
	_float3 vScale{};
	_float3 vRotation{};
	_float3 vTranslation{};
	unordered_map<string, vector<FIELD_DATA>> SlotDataValues;
}FACTORY_DESC;

typedef struct tagNPCSpec {
	string ProtoTag;
	function<CGameObject* ()> Create;
}NPC_SPEC;

void Create_Objects(const FACTORY_DESC& Desc);

void Create_ETC(const FACTORY_DESC& Desc);
void Create_NPC(const FACTORY_DESC& Desc);

NS_END
NS_END

