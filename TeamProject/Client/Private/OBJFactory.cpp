#include "pch.h"
#include "OBJFactory.h"
#include "GameInstance.h"
#include "MapTriggerObject.h"

#include "OfficeMeow.h"
#include "BangBooPay.h"
#include "BangBooAsk.h"
#include "BangBooDeliver.h"
#include "Howl.h"
#include "Jaeger.h"

void Client::Factory::Create_Interactable(const FACTORY_DESC& Desc)
{

	/* Portal */
	if (Desc.tagName == "Portal") {

	}
	/* ZeroPortal */
	else if (Desc.tagName == "ZeroPortal") {

	}
}

/* --------------------------------------------------------------------------------------------------------------------- */

static unordered_map<string, Factory::NPC_SPEC> s_NPCTable =
{
	{ "OfficeMeow",     Factory::NPC_SPEC{ "Proto_GameObject_OfficeMeow", &COfficeMeow::Create }},
	{ "BangBooDeliver", Factory::NPC_SPEC{ "Proto_GameObject_BangBooDeliver", &CBangBooDeliver::Create } },
	{ "BangBooPay",		Factory::NPC_SPEC{ "Proto_GameObject_BangBooPay", &CBangBooPay::Create } },
	{ "BangBooAsk",		Factory::NPC_SPEC{ "Proto_GameObject_BangBooAsk", &CBangBooAsk::Create } },
	{ "Howl",           Factory::NPC_SPEC{ "Proto_GameObject_Howl", &CHowl::Create } },
	{ "Jaeger",         Factory::NPC_SPEC{ "Proto_GameObject_Jaeger", &CJaeger::Create } }
};

/* Proto_GameObject¸¦ Ãß°¡ */
void Client::Factory::Create_NPC(const FACTORY_DESC& Desc)
{
	auto iter = s_NPCTable.find(Desc.tagName);

	if (iter == s_NPCTable.end())
		return;

	CCT_DESC CCT; 

	CCT.eGroup = COLLISION_GROUP::INTERACTABLE;
	CCT.iCollisionMask = 0xFFFFFFFF;
	CCT.bAutoFit = false;
	CCT.fHeight = 1.6f;
	CCT.fRadius = 0.4f;
	CCT.eGroup = COLLISION_GROUP::COMMON;
	CCT.vPos = Desc.vTranslation;

	PrototypeManager()->Add_ProtoType(Desc.tagLevel, iter->second.ProtoTag, iter->second.Create());

	auto OBJ = Builder::Create_Object({ Desc.tagLevel, iter->second.ProtoTag })
		.CharacterController(CCT)
		.Rotate(Desc.vRotation)
		.Build(Desc.tagName);

	ObjectManager()->Add_Object(OBJ, { Desc.tagLevel, Desc.tagName });
}
