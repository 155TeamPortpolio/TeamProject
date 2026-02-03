#include "pch.h"
#include "Scott_Level.h"
#include "Helper_Func.h"
#include "GameInstance.h"

#include "TestMap.h"
#include "TestObject.h"
#include "TestFloor.h"
#include "RigidBody.h"
#include "CharacterController.h"

#include "BattleSystem.h"
#include "FieldSystem.h"
#include "DataBase.h"

/* MapData */
#include "MapLoader.h"

/* Effect */
#include "MeshNode.h"
#include "SpriteNode.h"
#include "ParticleNode.h"
#include "TrailNode.h"
#include "EffectContainer.h"
#include "AttackSign.h"

// Camera
#include "CamDirector.h"

/* Character */
#include "Player.h"
#include "OfficeMeow.h"

/* Enemy */

/* UI */
#include "UIDirector.h"
#include "UI_MeshBillboard.h"

/* Interactable */
#include "Portal.h"

#include "Jaeger.h"

CScott_Level::CScott_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CScott_Level::Initialize()
{
	FieldSystem()->SetActive(true);
	return S_OK;
}

HRESULT CScott_Level::Awake()
{
	m_pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
	m_pPlayer->Set_PlayerType(CPlayer::PLAYER::FIELD);

	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(true);

	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	IResourceService* pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	auto objMgr = m_pGameInstance->Get_ObjectMgr();

	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Scott_Level");

	//==================== Interactable ===============
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_Portal", CPortal::Create());

	//============== Map ============================
	Ready_Map("Scott_Level", "Zero_Worksite");
	//Ready_Npc();

	CamDirector()->AutoField();

	return S_OK;
}

void CScott_Level::Update()
{
	FieldSystem()->Update();
}

HRESULT CScott_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CScott_Level::PreLoad_Level()
{
}

void CScott_Level::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	//Map Loader Logic is going to Change
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");

	Safe_Release(pMapLoader);
}

void CScott_Level::Ready_Npc()
{
	auto pProto = PrototypeManager();
	auto objMgr = ObjectManager();

	/*Npc*/
	CCT_DESC meowCCT;
	//meowCCT.eGroup = COLLISION_GROUP::PLAYER;
	meowCCT.iCollisionMask = 0xFFFFFFFF;
	//miyabiCCT.iCollisionMask = 0xFFFFFFFF & ~ENUM(COLLISION_GROUP::COMMON);
	meowCCT.bAutoFit = false;
	meowCCT.fHeight = 1.6f;
	meowCCT.fRadius = 0.4f;
	meowCCT.eGroup = COLLISION_GROUP::COMMON;
	//meowCCT.fBoundingMinY = -0.83f;
	meowCCT.vPos = { 0.f, -4.f, 0.f };
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_OfficeMeow", COfficeMeow::Create());
	auto testMeow = Builder::Create_Object({ "Scott_Level", "Proto_GameObject_OfficeMeow" })
		.CharacterController(meowCCT)
		.Rotate(_float3(0.f, 90.f, 0.f))
		.Build("Test_Meow");

	objMgr->Add_Object(testMeow, { "Scott_Level", "Npc_Layer" });
}

CScott_Level* CScott_Level::Create(const string& LevelKey)
{
	CScott_Level* instance = new CScott_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CScott_Level::Free()
{
	__super::Free();
	m_pGameInstance->DestroyInstance();
	m_pPlayer->Clear_Characters();
}
