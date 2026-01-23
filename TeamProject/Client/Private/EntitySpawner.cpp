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

/* Maptool Type 1 (ETC) */
#include "Portal.h"
#include "ZeroPortal.h"

/* --------------------------------------------------------------------------------------------------------------------- */

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
static unordered_map<string, Spawner::OBJ_SPEC> s_NPCTable =
{
	{ "OfficeMeow",     Spawner::OBJ_SPEC{ "Proto_GameObject_OfficeMeow", &COfficeMeow::Create }},
	{ "BangBooDeliver", Spawner::OBJ_SPEC{ "Proto_GameObject_BangBooDeliver", &CBangBooDeliver::Create } },
	{ "BangBooPay",		Spawner::OBJ_SPEC{ "Proto_GameObject_BangBooPay", &CBangBooPay::Create } },
	{ "BangBooAsk",		Spawner::OBJ_SPEC{ "Proto_GameObject_BangBooAsk", &CBangBooAsk::Create } },
	{ "Howl",           Spawner::OBJ_SPEC{ "Proto_GameObject_Howl", &CHowl::Create } },
	{ "Jaeger",         Spawner::OBJ_SPEC{ "Proto_GameObject_Jaeger", &CJaeger::Create } }
};

OBJECT_HANDLE Client::Spawner::Create_NPC(const SPAWNER_DESC& Desc)
{
	auto NPCTable = s_NPCTable.find(Desc.tagName);

	if (NPCTable == s_NPCTable.end())
		return OBJECT_HANDLE();

	CCT_DESC CCT; 

	CCT.eGroup = COLLISION_GROUP::COMMON;
	CCT.iCollisionMask = 0xFFFFFFFF;
	CCT.bAutoFit = false;
	CCT.fHeight = 1.6f;
	CCT.fRadius = 0.4f;
	CCT.vPos = Desc.vTranslation;

	PrototypeManager()->Add_ProtoType(Desc.tagLevel, NPCTable->second.ProtoTag, NPCTable->second.Create());

	auto Object = Builder::Create_Object({ Desc.tagLevel, NPCTable->second.ProtoTag })
		.CharacterController(CCT)
		.Rotate(Desc.vRotation)
		.Build(Desc.tagName);

	ObjectManager()->Add_Object(Object, { Desc.tagLevel, "NPC_Layer"});
	return Object->Get_Handle();
}
#pragma endregion
/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 1 */
#pragma region Entity1(Interactable)
static unordered_map<string, Spawner::OBJ_SPEC> s_InteractTable =
{
	{ "Portal",     Spawner::OBJ_SPEC{ "Proto_GameObject_Portal", &CPortal::Create }},
	{ "ZeroPortal", Spawner::OBJ_SPEC{ "Proto_GameObject_ZeroPortal", &CZeroPortal::Create }}
};

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
	tColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
	tColDesc.vCenter = { 0,0,0 };
	tColDesc.vSize = Desc.vScale;
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

	auto Object = Builder::Create_Object({ Desc.tagLevel, InteractTable->second.ProtoTag })
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

		character->Get_Component<CCharacterController>()->Set_Position(XMLoadFloat3(&Desc.vTranslation));
	}

	return OBJECT_HANDLE();
}
