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
#include "PaperEffect.h"

// Camera
#include "CamDirector.h"

/* Character */
#include "Player.h"
#include "OfficeMeow.h"

/* Enemy */

/* UI */
#include "UIDirector.h"
#include "UI_Party.h"

/* Interactable */
#include "Portal.h"
#include "Jaeger.h"

/*Room*/
#include "Room_Scott.h"
#include "Room_Party.h"

CScott_Level::CScott_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pFieldSystem{ CFieldSystem::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pFieldSystem);
}

HRESULT CScott_Level::Initialize()
{
	m_pFieldSystem->SetActive(true);
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
	Ready_UI();
	Ready_Map("Scott_Level", "Scott");
	//==================== Interactable ===============
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_Portal", CPortal::Create());

	//============== Map ============================
	//Effect
	pProto->Add_ProtoType("Scott_Level", "Proto_GameObject_PaperEffect", CPaperEffect::Create());
	auto PaperEffect = Builder::Create_Object({ "Scott_Level", "Proto_GameObject_PaperEffect" })
		.Position({ -2.f, -2.f, -6.f, })
		.Build("PaperEffect");
	ObjectManager()->Add_Object(PaperEffect, { "Scott_Level", "MapParticle_Layer" });

	CamDirector()->AutoField(CamStartDir::Back);

	_uint Version{};
	if (!RuntimeBucket().Int64.TryGet(PersistScope::SaveSlot, "Scott_Level", Version))
	{
		Version = 1;
		RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "Scott_Level", Version);
	}

	AudioDevice()->Set_Listener(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::FreeCam))->Get_Component<CTransform>());

	// 페이드인
	UIDirector()->FadeIn_Screen();

	return S_OK;
}

void CScott_Level::Update()
{
	m_pFieldSystem->Update();
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
	m_pFieldSystem->RegisterRoom(CRoom_Scott::Create({ "Scott" , true }));
	m_pFieldSystem->RegisterRoom(CRoom_Party::Create({ "Party" , false }));
	m_pFieldSystem->RequestEnter("Scott", true);
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

void CScott_Level::Ready_UI()
{
	if (FAILED(PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_Party", CUI_Party::Create())))
		return;

	auto pParty = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_Party" }).Build("party");
	if (pParty)
	{
		UIManager()->Add_UIObject(pParty, "Scott_Level");
		UIDirector()->Register(pParty);
	}
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
	if (m_pFieldSystem)
		m_pFieldSystem->SetActive(false);
	m_pFieldSystem->DestroyInstance();
	m_pGameInstance->DestroyInstance();
	m_pPlayer->Clear_Characters();
}
