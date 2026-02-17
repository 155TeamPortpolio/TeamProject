#include "pch.h"
#include "EntitySpawner.h"
#include "GameInstance.h"

#include "MapLoader_Helper.h"

#include "Player.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Collider.h"
#include "EffectContainer.h"


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
#include "BackgroundNpc.h"

/* Maptool Type 1 (Interactable) */
#include "Portal.h"
#include "ZeroPortal.h"
#include "UI_ZeroEntranceLogo.h"
#include "HealKit.h"
#include "BattleStarter.h"

/* Maptool Type 2 (ETC) */
#include "MilitaryHelicopter.h"
#include "DogFan.h"
#include "DogBone.h"
#include "ScottScreen.h"

/* Maptool Type 4 (InvWall) */
#include "MapInvisibleWall.h"

/* Maptool Type 5 (ETC) */
#include "Water.h"


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
	{ "Sirchop",		Spawner::OBJ_SPEC{ "Proto_GameObject_Sirchop", &CSirChop::Create } },
	{ "SilverAnbi",     Spawner::OBJ_SPEC{ "Proto_GameObject_SilverAnbi", &CSilverAnbi::Create } },
	{ "BackGround",     Spawner::OBJ_SPEC{ "Proto_GameObject_CBackgroundNpc", &CBackgroundNpc::Create } }
};

/* Maptool Type 1 */
static unordered_map<string, Spawner::OBJ_SPEC> s_InteractTable =
{
	{ "Portal",			Spawner::OBJ_SPEC{ "Proto_GameObject_Portal", &CPortal::Create }},
	{ "ZeroPortal",		Spawner::OBJ_SPEC{ "Proto_GameObject_ZeroPortal", &CZeroPortal::Create }},
	{ "UI_ZeroLogo",	Spawner::OBJ_SPEC{ "Proto_GameObject_ZeroEntranceLogo", &CUI_ZeroEntranceLogo::Create }},
	{ "HealKit",		Spawner::OBJ_SPEC{ "Proto_GameObject_HealKit", &CHealKit::Create }},
	{ "BattleStarter",	Spawner::OBJ_SPEC{ "Proto_GameObject_BattleStarter", &CBattleStarter::Create }}
};

/* Maptool Type 2 */
static unordered_map<string, Spawner::OBJ_SPEC> s_AmbientActorTable =
{
	{ "MilitaryHelicopter", Spawner::OBJ_SPEC{ "Proto_GameObject_MilitaryHelicopter", &CMilitaryHelicopter::Create }},
	{ "DogFan",				Spawner::OBJ_SPEC{ "Proto_GameObject_DogFan", &CDogFan::Create }},
	{ "DogBone",			Spawner::OBJ_SPEC{ "Proto_GameObject_DogBone", &CDogBone::Create }},
	{ "ScottScreen",		Spawner::OBJ_SPEC{ "Proto_GameObject_DogBone", &CScottScreen::Create }}
};

static unordered_map<string, Spawner::OBJ_SPEC> s_ETCTable =
{
	{ "Water",		Spawner::OBJ_SPEC{ "Water", &CWater::Create }},
};

#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

void Client::Spawner::Register_Prototype(const string& MapDataName, const string& PrototypeTag, function<CGameObject* ()> Create, ENTITY_TYPE EntityType)
{
	unordered_map<string, Spawner::OBJ_SPEC>* Table = { nullptr };

	switch (EntityType)
	{
	case Client::Spawner::ENTITY_TYPE::NPC:			Table = &s_NPCTable;			break;
	case Client::Spawner::ENTITY_TYPE::INTERACTABLE:Table = &s_InteractTable;		break;
	case Client::Spawner::ENTITY_TYPE::AMBIENTACTOR:Table = &s_AmbientActorTable;	break;
	case Client::Spawner::ENTITY_TYPE::EFFECT:									return;
	case Client::Spawner::ENTITY_TYPE::INVWALL:									return;
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
	case 2: return Create_AmbientActor(Desc);	break;
	case 3: return Create_Effect(Desc);			break;
	case 4: return Create_Invwall(Desc);		break;
	case 5:	return Create_ETC(Desc);			break;
	default:return OBJECT_HANDLE();				break;
	}
}

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 0 (NPC) */
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


	if (auto controller = Object->Get_Component<CCharacterController>())
		controller->Set_FootPosition(Desc.vTranslation);
	else
		Object->Get_Component<CTransform>()->Set_Pos(Desc.vTranslation);
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

/* Maptool Type 1 (Interactable) */
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
	tColDesc.bTrigger = true;
	tColDesc.vCenter = { 0,0,0 };
	tColDesc.vSize = Desc.vColSize;
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
	else if (Desc.tagName == "HealKit") {
		auto Slot = Desc.SlotDataValues.find("InteractSlot");

		if (Slot != Desc.SlotDataValues.end()) {
			for (auto tFieldData : Slot->second) {
				if (tFieldData.TagName == "KitType")
				{
					string KitType = *GetSlotValue<string>(tFieldData.defaultvalue);

					CHealKit::HEALKIT_DESC* pPortalDesc = new CHealKit::HEALKIT_DESC;
					tColDesc.eGroup = COLLISION_GROUP::COMMON;

					if (KitType == "HP")
						pPortalDesc->eItemType = CHealKit::ITEMTYPE::HP;
					else if (KitType == "Energy")
						pPortalDesc->eItemType = CHealKit::ITEMTYPE::ENERGY;
					else				
						pPortalDesc->eItemType = CHealKit::ITEMTYPE::END;

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
		.Scale(Desc.vScale)
		.Build(Desc.tagName);

	ObjectManager()->Add_Object(Object, { Desc.tagLevel, "InteractableObject_Layer" });
	return Object->Get_Handle();
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 2 (AmbientActor) */
#pragma region Entity2(AmbientActor)
OBJECT_HANDLE Client::Spawner::Create_AmbientActor(const SPAWNER_DESC& Desc)
{
	auto InteractTable = s_AmbientActorTable.find(Desc.tagName);
	if (InteractTable == s_AmbientActorTable.end())
		return OBJECT_HANDLE();

	auto Slot = Desc.SlotDataValues.find("AmbientActorSlot");

	CAmbientActor::AMBIENTACTOR_DESC* pAmbientActorDesc = new CAmbientActor::AMBIENTACTOR_DESC;

	if (Slot != Desc.SlotDataValues.end()) {
		CAmbientActor::AMBIENTACTOR_DESC* pAmbientActorDesc = new CAmbientActor::AMBIENTACTOR_DESC;

		for (auto tFieldData : Slot->second) {
			if (tFieldData.TagName == "AnimationName")
			{
				pAmbientActorDesc->strAnimName = *GetSlotValue<string>(tFieldData.defaultvalue);
			}
		}
	}
	
	PrototypeManager()->Add_ProtoType(Desc.tagLevel, InteractTable->second.ProtoTag, InteractTable->second.Create());

	CGameObject* Object = Builder::Create_Object({ Desc.tagLevel, InteractTable->second.ProtoTag })
		.Add_ObjDesc(pAmbientActorDesc)
		.Position(Desc.vTranslation)
		.Rotate(Desc.vRotation)
		.Scale(Desc.vScale)
		.Build(Desc.tagName);
	if (!Object)
		return OBJECT_HANDLE{};

	ObjectManager()->Add_Object(Object, { Desc.tagLevel, "AmbientActor_Layer" });
	return Object->Get_Handle();
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 3 (Effect) */
#pragma region Entity3(Effect)

OBJECT_HANDLE Client::Spawner::Create_Effect(const SPAWNER_DESC& Desc)
{
	auto pParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey, "Proto_GameObject_EffectContainer" })
		.Asset(Desc.tagName + ".json")
		.Position(Desc.vTranslation)
		.Rotate(Desc.vRotation)
		.Build(Desc.tagName);

	ObjectManager()->Add_Object(pParticle, { Desc.tagLevel, "MapParticle_Layer" });
	return pParticle->Get_Handle();
}

#pragma endregion
/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 4 (Invwall) */
#pragma region Entity4(Invwall)
OBJECT_HANDLE Client::Spawner::Create_Invwall(const SPAWNER_DESC& Desc)
{
	COLLIDER_DESC tColDesc{};

	tColDesc.eGroup = COLLISION_GROUP::COMMON;
	tColDesc.iCollisionMask = 0xFFFFFFFF;
	tColDesc.eType = COLLIDER_TYPE::BOX;
	tColDesc.bAutoFit = false;
	tColDesc.bTrigger = false;
	tColDesc.vCenter = { 0,0,0 };
	tColDesc.vSize = Desc.vColSize;
	tColDesc.vRotation = Desc.vRotation;

	CGameObject* Object = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_MapInvisibleWall" })
		.Position(Desc.vTranslation)
		.Collider(tColDesc)
		.Build(Desc.tagName);

#pragma region XWall
	auto Slot = Desc.SlotDataValues.find("XWall");

	if (Slot != Desc.SlotDataValues.end())
	{
		_vector2 vCount{}, vOffset{};
		_bool bCount{}, bOffset{};
	
		for (auto tFieldData : Slot->second)
		{
			if (tFieldData.TagName == "Count") {
				_vector2 vCount = *GetSlotValue<_float2>(tFieldData.defaultvalue);
				bCount = true;
			}
			if (tFieldData.TagName == "Offset") {
				_vector2 vOffset = *GetSlotValue<_float2>(tFieldData.defaultvalue);
				bOffset = true;
			}
		}

		if(bCount && bOffset)
			static_cast<CMapInvisibleWall*>(Object)->CreateXWall(vCount, vOffset);
	}
	
#pragma endregion

	ObjectManager()->Add_Object(Object, { Desc.tagLevel, "InvisibleWall_Layer" });
	return Object->Get_Handle();
}
#pragma endregion

/* --------------------------------------------------------------------------------------------------------------------- */

/* Maptool Type 5 (ETC) */
#pragma region Entity5(ETC)
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
	else {
		auto ETC = s_ETCTable.find(Desc.tagName);
		if (ETC == s_ETCTable.end())
			return OBJECT_HANDLE();

		PrototypeManager()->Add_ProtoType(Desc.tagLevel, ETC->second.ProtoTag, ETC->second.Create());

		CGameObject* Object = Builder::Create_Object({ Desc.tagLevel, ETC->second.ProtoTag })
			.Position(Desc.vTranslation)
			.Scale(Desc.vScale)
			.Build(Desc.tagName);

		ObjectManager()->Add_Object(Object, { Desc.tagLevel, "ETC_Layer" });
		return Object->Get_Handle();
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

