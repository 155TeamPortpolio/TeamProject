#include "pch.h"
#include "UI_TutorialVideo.h"

#include "GameInstance.h"
#include "Sprite2D.h"

#include "MFVideoDecoderBackend.h"

void CUI_TutorialVideo::Play(TUTORIAL_TYPE eType)
{
    if (!m_pPlayer || !m_pDecoder)
        return;

    // 0) 렌더가 이전 SRV 잡고 있을 수 있으니 끊기
    if (auto sprite = Get_Component<CSprite2D>())
        sprite->Set_Param("SpriteTexture", { nullptr, "Texture2D", 0 });

    // ? 1) 디코드 스레드를 먼저 멈추고 기다리기
  //  VideoService()->StopDecode(m_PlayerID);          // <- join까지 보장되게
    m_pDecoder->RequestStopDecode();                 // <- 안전망
    // 2) 플레이어 정리
    m_pPlayer->Stop();
    m_pPlayer->Close();

    // 3) 파일 경로 선택
    std::string filePath = Get_VideoPath(eType);

    // 4) 디코더 재오픈
    m_pDecoder->Close();
    if (!m_pDecoder->Open(filePath))
        return;
    m_pDecoder->SetLoop(false);

    // 5) 플레이어 오픈
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = filePath;
    desc.loop = true;
    m_pPlayer->Open(desc);

    // 6) 새 디코드 스레드 시작(반드시 새 세션으로)
    VideoService()->StartDecode(m_PlayerID);

    m_pPlayer->Play();
    Set_Alive(true);
}

HRESULT CUI_TutorialVideo::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_TutorialVideo::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    /*비디오를 읽는 디코더를 우선 생성*/
    m_pDecoder = CMFVideoDecoderBackend::Create();

    /*비디오를 플레이할 객체를 요청*/
    m_PlayerID = VideoService()->CreatePlayer(m_pDecoder);
    m_pPlayer = VideoService()->GetPlayer(m_PlayerID);

    /*비디오 플레이어에게 플레이할 영상을 알려줌*/
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = "../Bin/Resources/Video/Tutorial_ExtremeEvade.mp4";
    desc.loop = true;
    m_pPlayer->Open(desc); 

    VideoService()->StartDecode(m_PlayerID);

    /*아래는 셰이더. 리니어로 샘플링하면 색번짐 생김, 포인트 샘플로로*/
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("VideoPlay");

    m_vAnchorOffset = { 823.f, 172.f };
    m_vSize = { 457.f, 257.f};

    return S_OK;
}

void CUI_TutorialVideo::Awake()
{
}

void CUI_TutorialVideo::Update(_float dt)
{
    __super::Update(dt);

    auto pSRV = m_pPlayer->GetSRV();
    if (pSRV)
        Get_Component<CSprite2D>()->Set_Param("SpriteTexture", { pSRV  ,"Texture2D", 0 });
}

void CUI_TutorialVideo::UI_Active(void* pArg)
{
}

void CUI_TutorialVideo::UI_DeActive(void* pArg)
{
    m_pPlayer->Stop();
    Set_Alive(false);
}

string CUI_TutorialVideo::Get_VideoPath(TUTORIAL_TYPE eType)
{
    switch (eType)
    {
    case TUTORIAL_TYPE::EXTREME_EVADE: return "../Bin/Resources/Video/Tutorial_ExtremeEvade.mp4";// "../Bin/Resources/Video/Tutorial_GroggyCombo.mp4";
    case TUTORIAL_TYPE::EXTREME_SUPPORT: return "../Bin/Resources/Video/Tutorial_ExtremeSupport.mp4";
    case TUTORIAL_TYPE::DECIBEL_ULTIMATE: return "../Bin/Resources/Video/Tutorial_DecibelUltimate.mp4";
    case TUTORIAL_TYPE::GROGGY_COMBO: return "../Bin/Resources/Video/Tutorial_GroggyCombo.mp4";
    default: return "../Bin/Resources/Video/Tutorial_ExtremeEvade.mp4";
    }
}

CGameObject* CUI_TutorialVideo::Create()
{
    CUI_TutorialVideo* pInstance = new CUI_TutorialVideo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_TutorialVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_TutorialVideo::Clone(INIT_DESC* pArg)
{
    CUI_TutorialVideo* pInstance = new CUI_TutorialVideo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_TutorialVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_TutorialVideo::Free()
{
    __super::Free();
    /*비디오 서비스 통해 플레이어 아이디를 보내서 삭제.*/
    VideoService()->DestroyPlayer(m_PlayerID);
    /*디코더는 직접 삭제*/
    Safe_Release(m_pDecoder);
}