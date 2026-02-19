#include "pch.h"
#include "Tutorial_Level.h"

#include "GameInstance.h"
#include "CamDirector.h"
#include "UIDirector.h"

#include "BattleSystem.h"

#include "PostProcessCommand.h"
#include "PostRenderer.h"

#include "Light.h"

#include "MapLoader.h"
#include "Player.h"
#include "ProceduralSky.h"

/* UI */
#include "UI_TutorialInfo.h"

#include "Claymore.h"
#include "EnemyAttackCollider.h"

CTutorial_Level::CTutorial_Level(const string& LevelKey)
    :CLevel(LevelKey),
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CTutorial_Level::Initialize()
{
    return S_OK;
}

HRESULT CTutorial_Level::Awake()
{
    /* UI */
    Ready_UI();

    /* Map */
    Ready_Map("Tutorial_Level", "TrainingRoom");

    /* Player */
    m_pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
    m_pPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);

    BattleSystem()->SetBattleCharacters({CHARACTER::Corin });
    BattleSystem()->SetActive(true);

    /* Environment */
    auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));
    pCloud->Set_Alive(true);
    dynamic_cast<CProceduralSky*>(pCloud)->Set_CloudInfo({ _float3(0.151,0.109,0.074),_float3(0.15,0.158, 0.032), _float3(0.0,0.0,0.0),1.f,
        _float3(1.0,0.25,0.0),_float3(1.0,0.649, 0.0), 0.0 });

    auto pShadowCam = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::ShadowCam));
    auto pTransform = pShadowCam->Get_Component<CTransform>();
    pTransform->Set_Pos(_float4(0.f, 100.f, 0.f, 1.f));

    LIGHT_DESC lightDesc = {};
    lightDesc.vLightPosition = _float4(0.f, 50.f, 0.f, 1.f);
    lightDesc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    lightDesc.vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    lightDesc.vLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);
    lightDesc.fLightIntensity = 1.f;
    pShadowCam->Get_Component<CLight>()->Set_Desc(lightDesc, LIGHT_TYPE::DIRECTIONAL);

    auto pPost = RenderSystem()->GetPostRenderer();
    pPost->GetCommand<CFogCommand>()->
        SetEnable(false);

    /* Monster */
    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_Claymore", CClaymore::Create());
    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
    BattleSystem()->SpawnMosnter("Proto_GameObject_Claymore", _float3(-0.18f, 2.f, 1.59f));

    return S_OK;
}

void CTutorial_Level::Update()
{
    CBattleSystem::GetInstance()->Update();
}

HRESULT CTutorial_Level::Render()
{
    SetWindowText(g_hWnd, TEXT("Welcome To TutorialLevel"));
    return S_OK;
}

void CTutorial_Level::Ready_Map(const string& LevelTag, const string& AreaTag)
{
    CMapLoader* pMapLoader = CMapLoader::Create(LevelTag, AreaTag);
    if (nullptr == pMapLoader)
        MSG_BOX("Failed to Load MapData!");

    Safe_Release(pMapLoader);
}

void CTutorial_Level::Ready_UI()
{
    // ui 梓端 持失
    auto uiDirector = CUIDirector::GetInstance();
    uiDirector->Load_LevelObjects("Tutorial_Level");

    if (FAILED(PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialInfo", CUI_TutorialInfo::Create())))
        return;

    auto ptutorialInfo = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialInfo" })
        .Build("tutorialInfo");

    if (ptutorialInfo)
    {
        UIManager()->Add_UIObject(ptutorialInfo, "Tutorial_Level");
        UIDirector()->Register(ptutorialInfo);
    }

    // ui 実特
    uiDirector->FadeIn_Screen(1.f); 
    uiDirector->Show_HUD(CUIDirector::HUD::BATTLE);
    uiDirector->Show_TutorialInfo(TUTORIAL_TYPE::EXTREME_EVADE);
}

CTutorial_Level* CTutorial_Level::Create(const string& LevelKey)
{
    CTutorial_Level* instance = new CTutorial_Level(LevelKey);
    if (FAILED(instance->Initialize())) {
        MSG_BOX("Test level Create Failed");
        Safe_Release(instance);
    }

    return instance;
}

void CTutorial_Level::Free()
{
    __super::Free();
    m_pPlayer->Clear_Characters();
    m_pGameInstance->DestroyInstance();
}
