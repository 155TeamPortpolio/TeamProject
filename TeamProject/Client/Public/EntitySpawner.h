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
	_float3 vColSize{};
	unordered_map<string, vector<FIELD_DATA>> SlotDataValues;
}SPAWNER_DESC;

typedef struct tagOBJSpec {
	string ProtoTag;
	function<CGameObject* ()> Create;
}OBJ_SPEC;

enum class ENTITY_TYPE { NPC, INTERACTABLE, ETC, INVWALL };

//임시용으로 콜백함수받음 나중에없앨것
void Register_Prototype(const string& MapDataName, const string& PrototypeTag, function<CGameObject*()> Create, ENTITY_TYPE EntityaType);
OBJECT_HANDLE Create_Entity(const SPAWNER_DESC& Desc);

OBJECT_HANDLE Create_NPC(const SPAWNER_DESC& Desc);
OBJECT_HANDLE Create_Interactable(const SPAWNER_DESC& Desc);
OBJECT_HANDLE Create_ETC(const SPAWNER_DESC& Desc);
OBJECT_HANDLE Create_Invwall(const SPAWNER_DESC& Desc);

void Gravity(CGameObject* pGameObject,const vector<FIELD_DATA>& SlotDatas);

NS_END
NS_END

