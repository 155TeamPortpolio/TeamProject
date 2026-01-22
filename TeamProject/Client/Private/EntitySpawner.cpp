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

void Client::Spawner::Create_Entity(const SPAWNER_DESC& Desc)
{
	switch (Desc.iType)
	{
	case 0:	Create_NPC(Desc);			break;
	case 1:	Create_Interactable(Desc);	break;
	case 2:	Create_ETC(Desc);			break;
	default:
		break;
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

void Client::Spawner::Create_NPC(const SPAWNER_DESC& Desc)
{
	auto NPCTable = s_NPCTable.find(Desc.tagName);

	if (NPCTable == s_NPCTable.end())
		return;

	CCT_DESC CCT; 

	CCT.eGroup = COLLISION_GROUP::COMMON;
	CCT.iCollisionMask = 0xFFFFFFFF;
	CCT.bAutoFit = false;
	CCT.fHeight = 1.6f;
	CCT.fRadius = 0.4f;
	CCT.vPos = Desc.vTranslation;

	PrototypeManager()->Add_ProtoType(Desc.tagLevel, NPCTable->second.ProtoTag, NPCTable->second.Create());

	auto OBJ = Builder::Create_Object({ Desc.tagLevel, NPCTable->second.ProtoTag })
		.CharacterController(CCT)
		.Rotate(Desc.vRotation)
		.Build(Desc.tagName);

	ObjectManager()->Add_Object(OBJ, { Desc.tagLevel, "NPC_Layer"});
}
#pragma endregion
/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 1 */
#pragma region Entity1(Interactable)
static unordered_map<string, Spawner::OBJ_SPEC> s_ETCTable =
{
	{ "Portal",     Spawner::OBJ_SPEC{ "Proto_GameObject_Portal", &CPortal::Create }},
	{ "ZeroPortal", Spawner::OBJ_SPEC{ "Proto_GameObject_ZeroPortal", &CZeroPortal::Create }}
};

void Client::Spawner::Create_Interactable(const SPAWNER_DESC& Desc)
{
	auto ETCTable = s_ETCTable.find(Desc.tagName);

	if (ETCTable == s_ETCTable.end())
		return;

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
			return;
	}
#pragma endregion

	PrototypeManager()->Add_ProtoType(Desc.tagLevel, ETCTable->second.ProtoTag, ETCTable->second.Create());

	auto pObj = Builder::Create_Object({ Desc.tagLevel, ETCTable->second.ProtoTag })
		.Add_ObjDesc(pOBJDesc)
		.Position(Desc.vTranslation)
		.Collider(tColDesc)
		.Build(Desc.tagLevel);

	ObjectManager()->Add_Object(pObj, { Desc.tagLevel, "InteractableObject_Layer" });
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 2 */

void Client::Spawner::Create_ETC(const SPAWNER_DESC& Desc)
{
	/* PlayerSpawn */
	if (Desc.tagName == "PlayerSpawn") {
		auto player = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
		auto character = player->Get_CurCharacterHandle().Get();
		if (character)
			character->Get_Component<CCharacterController>()->Set_Position(XMLoadFloat3(&Desc.vTranslation));
	}
}
