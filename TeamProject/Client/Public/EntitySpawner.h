#pragma once
#include "Engine_Defines.h"
#include "MapTriggerObject.h"

NS_BEGIN(Client)
NS_BEGIN(Spawner)

typedef struct tagEntitySpawnerDesc {
	_int    iEntityID{};
	string  tagName{};
	string	tagLevel{};
	_int    iType{};
	_float3 vScale{};
	_float3 vRotation{};
	_float3 vTranslation{};
	unordered_map<string, vector<FIELD_DATA>> SlotDataValues;
}SPAWNER_DESC;

typedef struct tagOBJSpec {
	string ProtoTag;
	function<CGameObject* ()> Create;
}OBJ_SPEC;

void Create_Entity(const SPAWNER_DESC& Desc);

void Create_NPC(const SPAWNER_DESC& Desc);
void Create_Interactable(const SPAWNER_DESC& Desc);
void Create_ETC(const SPAWNER_DESC& Desc);
NS_END
NS_END

