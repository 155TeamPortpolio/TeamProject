#include "pch.h"
#include "Gacha_Level.h"
/* Camera */
#include "Camera.h"
#include "CamDirector.h"
#include "OrbitCam.h"
/* MapData */
#include "MapLoader.h"
/* UI */
#include "UIDirector.h"
/*GachaObject*/
#include "GachaProps.h"

CGacha_Level::CGacha_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pCamDirector{ CCamDirector::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGacha_Level::Initialize()
{
	return S_OK;
}

HRESULT CGacha_Level::Awake()
{
	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(false);

	Ready_GachaObjects();
	CamDirector()->SetSpaceRef(m_GachaHandle);

	return S_OK;
}

void CGacha_Level::Update()
{
}

HRESULT CGacha_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To GachaLevel"));
	return S_OK;
}

void CGacha_Level::Ready_GachaObjects()
{
	auto pProto = PrototypeManager();
	auto objMgr = ObjectManager();

	pProto->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaProps", CGachaProps::Create());
	auto gachaProps = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaProps" })
		.Build("GachaProps");

	objMgr->Add_Object(gachaProps, { "Gacha_Level", "Gacha_Layer" });

	m_GachaHandle = gachaProps->Get_Handle();
}

CGacha_Level* CGacha_Level::Create(const string& LevelKey)
{
	CGacha_Level* instance = new CGacha_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CGacha_Level::Free()
{
	__super::Free();
	m_pGameInstance->DestroyInstance();
}
