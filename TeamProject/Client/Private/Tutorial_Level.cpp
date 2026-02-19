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
    UIDirector()->FadeIn_Screen(1.f);

    Ready_Map("Tutorial_Level", "TrainingRoom");

    m_pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player)));
    m_pPlayer->Set_PlayerType(CPlayer::PLAYER::BATTLE);

    auto pCloud = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud));	

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

    return S_OK;
}

void CTutorial_Level::Update()
{
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
