#include "pch.h"
#include "MainCity_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "FieldSystem.h"
#include "Room_Street.h"

// Camera
#include "Camera.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "ShadowCam.h"

/* MapData */
#include "MapLoader.h"

#include "Player.h"

/* UI */
#include "UIDirector.h"

CMainCity_Level::CMainCity_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainCity_Level::Initialize()
{
	return S_OK;
}

HRESULT CMainCity_Level::Awake()
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
	uiDirector->Load_LevelObjects("MainCity_Level");

	Ready_Map("MainCity_Level", "MainCity");

	CamDirector()->AutoField();
	FieldSystem()->SetActive(true);

	return S_OK;
}

void CMainCity_Level::Update()
{
	FieldSystem()->Update();
}

HRESULT CMainCity_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CMainCity_Level::PreLoad_Level()
{
}

void CMainCity_Level::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	FieldSystem()->RegisterRoom(CRoom_Street::Create({ "MainCity" , true }));
	FieldSystem()->RequestEnter("MainCity", true);
}

CMainCity_Level* CMainCity_Level::Create(const string& LevelKey)
{
	CMainCity_Level* instance = new CMainCity_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CMainCity_Level::Free()
{
	__super::Free();
	m_pGameInstance->DestroyInstance();
}
