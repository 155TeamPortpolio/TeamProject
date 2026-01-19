#include "pch.h"
#include "OBJFactory.h"
#include "GameInstance.h"
#include "MapTriggerObject.h"

#include "OfficeMeow.h"
#include "BangBooPay.h"
#include "BangBooAsk.h"
#include "BangBooDeliver.h"

void Client::Factory::Create_Interactable(const CMapTriggerObject::MAP_TRIGGEROBJ_DESC* Desc)
{

	/* Portal */
	if (Desc->InstanceName == "Portal") {

	}
}

/* --------------------------------------------------------------------------------------------------------------------- */
void Client::Factory::Create_NPC(const CMapTriggerObject::MAP_TRIGGEROBJ_DESC* Desc)
{
	CCT_DESC CCT;
	/* OfficeMeow */
	if (Desc->InstanceName == "OfficeMeow") {
	
		CCT.eGroup = COLLISION_GROUP::INTERACABLE;
		CCT.iCollisionMask = 0xFFFFFFFF;
		CCT.bAutoFit = false;
		CCT.fHeight = 1.6f;
		CCT.fRadius = 0.4f;
		CCT.eGroup = COLLISION_GROUP::COMMON;
		CCT.vPos = Desc->vPos;
		
		PrototypeManager()->Add_ProtoType(Desc->TagLevel, "Proto_GameObject_OfficeMeow", COfficeMeow::Create());
		auto OfficeMeow = Builder::Create_Object({ Desc->TagLevel, "Proto_GameObject_OfficeMeow" })
			.CharacterController(CCT)
			.Rotate(Desc->vLook)
			.Build("OfficeMeow");

		ObjectManager()->Add_Object(OfficeMeow, { Desc->TagLevel, Desc->InstanceName });
	}
	/* BangBoo_Deliver */
	else if (Desc->InstanceName == "Deliver") {
		CCT.eGroup = COLLISION_GROUP::INTERACABLE;
		CCT.iCollisionMask = 0xFFFFFFFF;
		CCT.bAutoFit = false;
		CCT.fHeight = 1.6f;
		CCT.fRadius = 0.4f;
		CCT.eGroup = COLLISION_GROUP::COMMON;
		CCT.vPos = Desc->vPos;

		PrototypeManager()->Add_ProtoType(Desc->TagLevel, "Proto_GameObject_BangBooDeliver", CBangBooAsk::Create());
		auto BangBoo_Deliver = Builder::Create_Object({ Desc->TagLevel, "Proto_GameObject_BangBooDeliver" })
			.CharacterController(CCT)
			.Rotate(Desc->vLook)
			.Build("BangBoo_Deliver");

		ObjectManager()->Add_Object(BangBoo_Deliver, { Desc->TagLevel, Desc->InstanceName });
	}
	/* BangBoo_Ask */
	else if (Desc->InstanceName == "Ask") {
		CCT.eGroup = COLLISION_GROUP::INTERACABLE;
		CCT.iCollisionMask = 0xFFFFFFFF;
		CCT.bAutoFit = false;
		CCT.fHeight = 1.6f;
		CCT.fRadius = 0.4f;
		CCT.eGroup = COLLISION_GROUP::COMMON;
		CCT.vPos = Desc->vPos;

		PrototypeManager()->Add_ProtoType(Desc->TagLevel, "Proto_GameObject_BangBooAsk", CBangBooDeliver::Create());
		auto BangBoo_Ask = Builder::Create_Object({ Desc->TagLevel, "Proto_GameObject_BangBooAsk" })
			.CharacterController(CCT)
			.Rotate(Desc->vLook)
			.Build("BangBoo_Ask");

		ObjectManager()->Add_Object(BangBoo_Ask, { Desc->TagLevel, Desc->InstanceName });
	}
	/* BangBoo_Pay */
	else if (Desc->InstanceName == "Pay") {
		CCT.eGroup = COLLISION_GROUP::INTERACABLE;
		CCT.iCollisionMask = 0xFFFFFFFF;
		CCT.bAutoFit = false;
		CCT.fHeight = 1.6f;
		CCT.fRadius = 0.4f;
		CCT.eGroup = COLLISION_GROUP::COMMON;
		CCT.vPos = Desc->vPos;

		PrototypeManager()->Add_ProtoType(Desc->TagLevel, "Proto_GameObject_BangBooPay", CBangBooPay::Create());
		auto BangBoo_Pay = Builder::Create_Object({ Desc->TagLevel, "Proto_GameObject_BangBooPay" })
			.CharacterController(CCT)
			.Rotate(Desc->vLook)
			.Build("BangBoo_Pay");

		ObjectManager()->Add_Object(BangBoo_Pay, { Desc->TagLevel, Desc->InstanceName });
	}
}