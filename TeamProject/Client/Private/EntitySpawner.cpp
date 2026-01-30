#include "pch.h"
#include "EntitySpawner.h"
#include "GameInstance.h"

#include "MapLoader_Helper.h"

#include "Player.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Collider.h"

/* Maptool Type 0 (NPC) */
#include "OfficeMeow.h"
#include "BangBooPay.h"
#include "BangBooAsk.h"
#include "BangBooDeliver.h"
#include "Howl.h"
#include "Jaeger.h"
#include "SirChop.h"
#include "ElectricBoo.h"
#include "SilverAnbi.h"

/* Maptool Type 1 (ETC) */
#include "Portal.h"
#include "ZeroPortal.h"

#pragma region Tables
/* Maptool Type 0 */
static unordered_map<string, Spawner::OBJ_SPEC> s_NPCTable =
{
	{ "OfficeMeow",     Spawner::OBJ_SPEC{ "Proto_GameObject_OfficeMeow", &COfficeMeow::Create }},
	{ "BangBooDeliver", Spawner::OBJ_SPEC{ "Proto_GameObject_BangBooDeliver", &CBangBooDeliver::Create } },
	{ "BangBooPay",		Spawner::OBJ_SPEC{ "Proto_GameObject_BangBooPay", &CBangBooPay::Create } },
	{ "BangBooAsk",		Spawner::OBJ_SPEC{ "Proto_GameObject_BangBooAsk", &CBangBooAsk::Create } },
	{ "Howl",           Spawner::OBJ_SPEC{ "Proto_GameObject_Howl", &CHowl::Create } },
	{ "Jaeger",         Spawner::OBJ_SPEC{ "Proto_GameObject_Jaeger", &CJaeger::Create } },
	{ "ExploreBoo",     Spawner::OBJ_SPEC{ "Proto_GameObject_ExploreBoo", &CElectricBoo::Create } },
	{ "Sirchop",     Spawner::OBJ_SPEC{ "Proto_GameObject_Sirchop", &CSirChop::Create } },
	{ "SilverAnbi",     Spawner::OBJ_SPEC{ "Proto_GameObject_SilverAnbi", &CSilverAnbi::Create } }
};

/* Maptool Type 1 */
static unordered_map<string, Spawner::OBJ_SPEC> s_InteractTable =
{
	{ "Portal",     Spawner::OBJ_SPEC{ "Proto_GameObject_Portal", &CPortal::Create }},
	{ "ZeroPortal", Spawner::OBJ_SPEC{ "Proto_GameObject_ZeroPortal", &CZeroPortal::Create }}
};
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

void Client::Spawner::Register_Prototype(const string& MapDataName, const string& PrototypeTag, function<CGameObject* ()> Create, ENTITY_TYPE EntityType)
{
	unordered_map<string, Spawner::OBJ_SPEC>* Table = { nullptr };

	switch (EntityType)
	{
	case Client::Spawner::ENTITY_TYPE::NPC:			Table = &s_NPCTable;		break;
	case Client::Spawner::ENTITY_TYPE::INTERACTABLE:Table = &s_InteractTable;	break;
	case Client::Spawner::ENTITY_TYPE::ETC:										return;
	default:																	return;
	}

	Table->emplace(MapDataName, Spawner::OBJ_SPEC{ PrototypeTag, Create });
}


OBJECT_HANDLE Client::Spawner::Create_Entity(const SPAWNER_DESC& Desc)
{
	switch (Desc.iType)
	{
	case 0:	return Create_NPC(Desc);			break;
	case 1:	return Create_Interactable(Desc);	break;
	case 2:	return Create_ETC(Desc);			break;
	default:return OBJECT_HANDLE();				break;
	}
}

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 0 */
#pragma region Entity0(NPC)
OBJECT_HANDLE Client::Spawner::Create_NPC(const SPAWNER_DESC& Desc)
{
	auto NPCTable = s_NPCTable.find(Desc.tagName);

	if (NPCTable == s_NPCTable.end())
		return OBJECT_HANDLE();

	CCT_DESC CCT; 

	CCT.eGroup = COLLISION_GROUP::INTERACTABLE;
	CCT.iCollisionMask = 0xFFFFFFFF;
	CCT.bAutoFit = false;
	CCT.fRadius = (Desc.vColSize.x +Desc.vColSize.z) * 0.25f;
	CCT.fHeight = (Desc.vColSize.y * 0.5f) - CCT.fRadius;
	
	CCT.vPos = _float3(Desc.vTranslation.x, Desc.vTranslation.y, Desc.vTranslation.z);
	
	PrototypeManager()->Add_ProtoType(Desc.tagLevel, NPCTable->second.ProtoTag, NPCTable->second.Create());

	CGameObject* Object = Builder::Create_Object({ Desc.tagLevel, NPCTable->second.ProtoTag })
		.CharacterController(CCT)
		.Rotate(Desc.vRotation)
		.Scale(Desc.vScale)
		.Build(Desc.tagName);

	Object->Get_Component<CCharacterController>()->Set_FootPosition(Desc.vTranslation);
	
	//Optional
	auto iter = Desc.SlotDataValues.find("NPCSlot");
	if (iter != Desc.SlotDataValues.end()) {
		Spawner::Gravity(Object, iter->second);
	}

	ObjectManager()->Add_Object(Object, { Desc.tagLevel, "NPC_Layer"});
	return Object->Get_Handle();
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 1 */
#pragma region Entity1(Interactable)
OBJECT_HANDLE Client::Spawner::Create_Interactable(const SPAWNER_DESC& Desc)
{
	auto InteractTable = s_InteractTable.find(Desc.tagName);

	if (InteractTable == s_InteractTable.end())
		return OBJECT_HANDLE();

	COLLIDER_DESC tColDesc{};
	GAMEOBJECT_DESC* pOBJDesc = { nullptr };

	tColDesc.eGroup = COLLISION_GROUP::INTERACTABLE;
	tColDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
	tColDesc.eType = COLLIDER_TYPE::BOX;
	tColDesc.bAutoFit = false;
	tColDesc.bTrigger = true; // �浹 �ڽ� �����ϴ� Ʈ����
	tColDesc.vCenter = { 0,0,0 };
	tColDesc.vSize = Desc.vRotation;
	tColDesc.vRotation = Desc.vRotation;

#pragma region Exception
	/* Portal */
	if (Desc.tagName == "Portal")
	{
		auto Slot = Desc.SlotDataValues.find("InteractSlot");

		if (Slot != Desc.SlotDataValues.end()) {
			for (auto tFieldData : Slot->second) {
				if (tFieldData.TagName == "NextLevel")
				{
					string NextLevelTag = *GetSlotValue<string>(tFieldData.defaultvalue);

					CPortal::PORTAL_DESC* pPortalDesc = new CPortal::PORTAL_DESC;
					pPortalDesc->InstanceName = "Portal" + NextLevelTag;
					pPortalDesc->NextNameTag = NextLevelTag;
					pOBJDesc = pPortalDesc;
				}
			}
		}

		if (nullptr == pOBJDesc)
			return OBJECT_HANDLE();
	}
#pragma endregion

	PrototypeManager()->Add_ProtoType(Desc.tagLevel, InteractTable->second.ProtoTag, InteractTable->second.Create());

	CGameObject* Object = Builder::Create_Object({ Desc.tagLevel, InteractTable->second.ProtoTag })
		.Add_ObjDesc(pOBJDesc)
		.Position(Desc.vTranslation)
		.Collider(tColDesc)
		.Build(Desc.tagLevel);

	ObjectManager()->Add_Object(Object, { Desc.tagLevel, "InteractableObject_Layer" });
	return Object->Get_Handle();
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 2 */
#pragma region Entity2(ETC)
OBJECT_HANDLE Client::Spawner::Create_ETC(const SPAWNER_DESC& Desc)
{
	/* PlayerSpawn */
	if (Desc.tagName == "PlayerSpawn") {
		auto player = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
		auto character = player->Get_CurCharacterHandle().Get();
		if (!character)
			return OBJECT_HANDLE();
			
		character->Get_Component<CTransform>()->Set_Quaternion(
			XMQuaternionRotationRollPitchYaw(
				Desc.vRotation.x,
				Desc.vRotation.y,
				Desc.vRotation.z));

		character->Get_Component<CCharacterController>()->Set_FootPosition(XMLoadFloat3(&Desc.vTranslation));
	}

	return OBJECT_HANDLE();
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Function */

#pragma region Functions
void Client::Spawner::Gravity(CGameObject* pGameObject, const vector<FIELD_DATA>& SlotDatas)
{
	for (const auto& Data : SlotDatas) {
		if (Data.TagName == "Gravity") {
			pGameObject->Get_Component<CCharacterController>()->Set_Gravity(*GetSlotValue<_bool>(Data.defaultvalue));
			return;
		}
	}
}
#pragma endregion
