#include "pch.h"
#include "Gacha_Level.h"
/* Camera */
#include "Camera.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "ShadowCam.h"
/* MapData */
#include "MapLoader.h"
/* UI */
#include "UIDirector.h"
/*GachaObject*/
#include "GachaProps.h"
/*Component*/
#include "Light.h"
/*DataBase*/
#include "DataBase.h"

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
	Ready_Map("Gacha_Level", "Gacha");

	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(false);

	auto pShadowCam = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::ShadowCam));
	LIGHT_DESC lightDesc = {};
	lightDesc.vLightPosition = _float4(-50.f, 50.f, 0.f, 1.f);
	lightDesc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.vLightAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	lightDesc.vLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.fLightIntensity = 1.f;
	pShadowCam->Get_Component<CLight>()->Set_Desc(lightDesc, LIGHT_TYPE::DIRECTIONAL);

	RenderSystem()->Set_FogDesc({ _float4(0.1f, 0.1f, 0.1f, 1.0f) ,0.f, 0.f, 0.02f, false });

	Ready_GachaObjects();

	// Camera
	CamDirector()->SetSpaceRef(m_GachaHandle);
	CamDirector()->RequestSequence("Gacha/Down");

	m_pGachaProps->SetupInitialSequence();

	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Gacha_Level");

	return S_OK;
}

void CGacha_Level::Update()
{
	Update_CamTime();
}

HRESULT CGacha_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To GachaLevel"));
	return S_OK;
}

void CGacha_Level::Ready_Map(const string& LevelTag, const string& AreaTag)
{
	CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
	if (nullptr == pMapLoader)
		MSG_BOX("Failed to Load MapData!");

	Safe_Release(pMapLoader);
}

void CGacha_Level::Ready_GachaObjects()
{
	m_ResultDesc = CDataBase::GetInstance()->GetGachaGroup();

	auto pProto = PrototypeManager();
	auto objMgr = ObjectManager();

	pProto->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaProps", CGachaProps::Create(&m_ResultDesc));
	auto gachaProps = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaProps" })
		.Build("GachaProps");

	objMgr->Add_Object(gachaProps, { "Gacha_Level", "Gacha_Layer" });

	m_GachaHandle = gachaProps->Get_Handle();
	m_pGachaProps = dynamic_cast<CGachaProps*>(gachaProps);
}

void CGacha_Level::Update_CamTime()
{
	if (CamDirector()->GetCurSeqName() == "Gacha/Down")
	{
		if (CamDirector()->GetTime() >= 2.2f)
			m_pGachaProps->PlayTVSequence();
	}
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
