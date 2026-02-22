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
#include "UI_GachaDisplay.h"
/*GachaObject*/
#include "GachaProps.h"
/*Component*/
#include "Light.h"
/*DataBase*/
#include "DataBase.h"

#include "EffectContainer.h"

#include "PostProcessCommand.h"
#include "PostRenderer.h"

#include "FieldSystem.h"

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
	//==================== UI ===============
	UIDirector()->Load_LevelObjects("Gacha_Level");
	UIDirector()->FadeIn_Screen(1.f);

	Ready_Map("Gacha_Level", "Gacha");

	auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
	pCloud->Set_Alive(false);

	auto pShadowCam = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::ShadowCam));
	
	auto pTransform = pShadowCam->Get_Component<CTransform>();
	pTransform->Set_Pos(_float4(0.f, 50.f, -50.f, 1.f));

	LIGHT_DESC lightDesc = {};
	lightDesc.vLightPosition = _float4(0.f, 50.f, 0.f, 1.f);
	lightDesc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f); 
	lightDesc.vLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.fLightIntensity = 1.f;
	pShadowCam->Get_Component<CLight>()->Set_Desc(lightDesc, LIGHT_TYPE::DIRECTIONAL);

	Ready_GachaObjects(); 
	FieldSystem()->PlayBGM("Gacha_Theme.wav");
	//==================== Effect ============
	//auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
	//	.Asset("gacha_background_light.json")
	//	.Position(_float3(0.f, 0.5f, -0.1f))
	//	.Build("Gacha_Light");
	//
	//ObjectManager()->Add_Object(pEffect, { "Gacha_Level","Effect_Layer" });
	RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "MainCity_Level", 2);

	auto pPost = RenderSystem()->GetPostRenderer();
	pPost->GetCommand<CFogCommand>()->
		SetEnable(false);

	Ready_GachaUI();	// UIDirector에서 Load_LevelObject 실행 한 뒤에

	return S_OK;
}

void CGacha_Level::Update()
{
	Set_ShadowCam();
	Update_CamTime();
	Update_AvatarSequence();
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

void CGacha_Level::Ready_GachaUI()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaDisplay", CUI_GachaDisplay::Create());

	CUI_GachaDisplay::DISPLAY_INIT_DESC* pDesc = new CUI_GachaDisplay::DISPLAY_INIT_DESC;
	pDesc->eGrade = Get_HigestGrade();		// 뽑은 아이템에서 가장 높은 등급 넣기
	pDesc->onClickSkip = [&]() {			// 스킵 눌렀을 때 실행할 함수 넣기
		m_pGachaProps->ShowResults();
		//MSG_BOX("Skip"); 
		};
	pDesc->onVideoFinished = [&]() {		// 비디오 끝났을 때 실행할 함수 넣기
		Play_CameraSequence();
		//MSG_BOX("VideoFinished"); 
		}; 

	auto pGachaDisplay = Builder::Create_UIObject({ "Gacha_Level", "Proto_GameObject_GachaDisplay"})
		.Add_UIDesc(pDesc)
		.Build("gacha_display");

	if (!pGachaDisplay)
		return;

	UIManager()->Add_UIObject(pGachaDisplay, "Gacha_Level");
	UIDirector()->Register(pGachaDisplay);
}

void CGacha_Level::Update_CamTime()
{
	if (CamDirector()->GetCurSeqName() == "Gacha/Down")
	{
		if (CamDirector()->GetTime() >= 2.2f)
			m_pGachaProps->PlayTVSequence();
	}
}

void CGacha_Level::Set_ShadowCam()
{
	auto pShadowCam = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::ShadowCam));

	auto pTransform = pShadowCam->Get_Component<CTransform>();
	pTransform->Set_Pos(_float4(0.f, 50.f, -50.f, 1.f));

	LIGHT_DESC lightDesc = {};
	lightDesc.vLightPosition = _float4(0.f, 50.f, 0.f, 1.f);
	lightDesc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.vLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.fLightIntensity = 1.f;
	pShadowCam->Get_Component<CLight>()->Set_Desc(lightDesc, LIGHT_TYPE::DIRECTIONAL);
}

void CGacha_Level::Update_AvatarSequence()
{
	auto& cam = *CamDirector();

	if (cam.IsFinished(CamEventType::Miyabi_01_Finished))
		cam.RequestSequence("Gacha/Miyabi_02");

	if (cam.IsFinished(CamEventType::Miyabi_02_Finished))
		cam.RequestSequence("Gacha/Miyabi_03");

	if (cam.IsFinished(CamEventType::Miyabi_03_Finished))
		cam.RequestSequence("Gacha/Miyabi_01");

	if (cam.IsFinished(CamEventType::JaneDoe_01_Finished))
		cam.RequestSequence("Gacha/JaneDoe_02");

	if (cam.IsFinished(CamEventType::JaneDoe_02_Finished))
		cam.RequestSequence("Gacha/JaneDoe_03");

	if (cam.IsFinished(CamEventType::JaneDoe_03_Finished))
		cam.RequestSequence("Gacha/JaneDoe_01");
}

void CGacha_Level::Play_CameraSequence()
{
	UIDirector()->FadeIn_Screen(1.5f);
	CamDirector()->SetSpaceRef(m_GachaHandle);
	CamDirector()->RequestSequence("Gacha/Down");

	m_pGachaProps->SetupInitialSequence();
}

GachaGrade CGacha_Level::Get_HigestGrade()
{
	if (m_ResultDesc.empty())
		return GachaGrade::B; 
	GachaGrade highest = m_ResultDesc[0].Grade;

	auto result = std::min_element(m_ResultDesc.begin(), m_ResultDesc.end(),
		[](const auto& a, const auto& b) {
			return a.Grade < b.Grade;
		});

	return result->Grade;
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
	FieldSystem()->FadeOutBGM();
	m_pGameInstance->DestroyInstance();
}
