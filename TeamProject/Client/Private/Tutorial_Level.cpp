#include "pch.h"
#include "Tutorial_Level.h"

#include "GameInstance.h"
#include "CamDirector.h"
#include "UIDirector.h"

#include "BattleSystem.h"

#include "PostProcessCommand.h"
#include "PostRenderer.h"

#include "AudioSource.h"

#include "Light.h"

#include "MapLoader.h"
#include "Player.h"
#include "ProceduralSky.h"

/* UI */
#include "UI_TutorialInfo.h"
#include "UI_TutorialGuide.h"

#include "Claymore.h"
#include "EnemyAttackCollider.h"

CTutorial_Level::CTutorial_Level(const string& LevelKey)
    :CLevel(LevelKey),
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);

    m_pBGM = CAudioSource::Create();
    m_pBGM->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Global/BGM");
}

HRESULT CTutorial_Level::Initialize()
{
    return S_OK;
}

HRESULT CTutorial_Level::Awake()
{
    /* Map */
    Ready_Map("Tutorial_Level", "TrainingRoom");

    /* Player */
    m_pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
    m_pPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);

    BattleSystem()->SetBattleCharacters({CHARACTER::Corin, CHARACTER::JaneDoe });
    BattleSystem()->SetActive(true);
    /* Environment */
    auto pCloud = dynamic_cast<CProceduralSky*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud)));
    pCloud->Set_Alive(true);
    m_PrevCloud = pCloud->Get_CloudInfo();
    pCloud->Set_CloudInfo({ _float3(0.151,0.109,0.074),_float3(0.15,0.158, 0.032), _float3(0.0,0.0,0.0),1.f,
        _float3(1.0,0.25,0.0),_float3(1.0,0.649, 0.0), 0.0 });

    auto pShadowCam = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::ShadowCam));
    auto pTransform = pShadowCam->Get_Component<CTransform>();
    pTransform->Set_Pos(_float4(0.f, 100.f, 0.f, 1.f));

    CUIDirector::GetInstance()->Show_HUD(CUIDirector::HUD::BATTLE);

    RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "Scott_Level", 2);
    
    LIGHT_DESC lightDesc = {};
    lightDesc.vLightPosition = _float4(0.f, 50.f, 0.f, 1.f);
    lightDesc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    lightDesc.vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    lightDesc.vLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);
    lightDesc.fLightIntensity = 1.f;
    pShadowCam->Get_Component<CLight>()->Set_Desc(lightDesc, LIGHT_TYPE::DIRECTIONAL);

    auto pPost = RenderSystem()->GetPostRenderer();
    auto pFogCommand = pPost->GetCommand<CFogCommand>();
    m_PrevFog = pFogCommand->GetFogDesc();
    m_bPrevFogUse = pFogCommand->IsEnabled();
    pFogCommand->SetEnable(false);

    /* Monster */
    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_Claymore", CClaymore::Create());
    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_EnemyAttackCollider", CEnemyAttackCollider::Create());
    BattleSystem()->SpawnMosnter("Proto_GameObject_Claymore", _float3(-0.18f, 2.f, 1.59f));

    CamDirector()->AutoBattle(CamStartDir::Back);
     
    /* UI */
    Ready_UI();

    m_pBGM->Slot("TutorialBGM.wav").Group(SOUND_GROUP::BGM).Attribute3D(false).Volume(0.2f).Loop(true).Play();

    /**/
    TUTORIAL_DESC desc = {};
    desc.eType = TUTORIAL_TYPE::EXTREME_EVADE;
    desc.eState = TUTORIAL_STATE::INFO;
    EventSystem()->Broadcast<TUTORIAL_DESC>({ desc });

    // 페이드인
    UIDirector()->FadeIn_Screen();

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
    // ui ��ü ����
    auto uiDirector = CUIDirector::GetInstance();
    uiDirector->Load_LevelObjects("Tutorial_Level");

    if (FAILED(PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialInfo", CUI_TutorialInfo::Create())))
        return;
    if (FAILED(PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialGuide", CUI_TutorialGuide::Create())))
        return;

    auto ptutorialInfo = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialInfo" })
        .Build("tutorialInfo");
    if (ptutorialInfo)
    {
        UIManager()->Add_UIObject(ptutorialInfo, "Tutorial_Level");
        UIDirector()->Register(ptutorialInfo);
    }

    auto ptutorialGuide = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialGuide" })
        .Build("tutorialGuide");
    if (ptutorialGuide)
    {
        UIManager()->Add_UIObject(ptutorialGuide, "Tutorial_Level");
        UIDirector()->Register(ptutorialGuide);
    }

    // ui ����
    uiDirector->FadeIn_Screen(1.f); 
    uiDirector->Show_HUD(CUIDirector::HUD::BATTLE);
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
    m_pBGM->FadeOut_Volume("TutorialBGM.wav", 0.9);

    auto pCloud = dynamic_cast<CProceduralSky*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud)));
    pCloud->Set_CloudInfo(m_PrevCloud);

    auto pPost = RenderSystem()->GetPostRenderer();
    pPost->GetCommand<CFogCommand>()
        ->SetFogDesc(m_PrevFog)
        ->SetEnable(m_bPrevFogUse);

    m_pPlayer->Clear_Characters();
    m_pGameInstance->DestroyInstance();
    BattleSystem()->SetActive(false);
    Safe_Release(m_pBGM);
}
