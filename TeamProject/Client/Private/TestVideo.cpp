#include "pch.h"
#include "TestVideo.h"
#include "GameInstance.h"
#include "MFVideoDecoderBackend.h"
#include "Sprite2D.h"

CTestVideo::CTestVideo()
{
}

HRESULT CTestVideo::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CTestVideo::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    /*비디오를 읽는 디코더를 우선 생성*/
    m_pDecoder = CMFVideoDecoderBackend::Create();

    /*비디오를 플레이할 객체를 요청*/
    m_PlayerID = VideoService()->CreatePlayer(m_pDecoder);
    m_pPlayer = VideoService()->GetPlayer(m_PlayerID);

    /*비디오 플레이어에게 플레이할 영상을 알려줌*/
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = "../Bin/Resources/Video/Test (2).mp4"; 
    desc.loop = true;
    m_pPlayer->Open(desc);

    /*재생 / 디코딩 시작*/
    m_pPlayer->Play();
    VideoService()->StartDecode(m_PlayerID);

    /*아래는 셰이더. 리니어로 샘플링하면 색번짐 생김, 포인트 샘플로로*/
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("VideoPlay");
    m_vSize = { 1600 ,900};
    return S_OK;
}

void CTestVideo::Awake()
{
}

void CTestVideo::Update(_float dt)
{
    Get_Component<CSprite2D>()->Set_Param("SpriteTexture", { m_pPlayer->GetSRV(),"Texture2D",0 });
    if (InputDevice()->Key_Tap(VK_SPACE)) {
        if(m_pPlayer->GetState() == VIDEO_PLAY_STATE::Paused)
            m_pPlayer->Play(); /*상태를 꺼내와서 확인 후 플레이,퍼즈,클로즈 가능*/
        else
            m_pPlayer->Pause();
    }

}

void CTestVideo::UI_Active(void* pArg)
{
    if (!pArg)
        return;

}

void CTestVideo::UI_DeActive(void* pArg)
{
 
}

CTestVideo* CTestVideo::Create()
{
    CTestVideo* instance = new CTestVideo();

    if (FAILED(instance->Initialize_Prototype()))
    {
        Safe_Release(instance);
        MSG_BOX("Failed to create : CTestVideo");
    }

    return instance;
}

CGameObject* CTestVideo::Clone(INIT_DESC* pArg)
{
    CTestVideo* pInstance = new CTestVideo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CTestVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTestVideo::Free()
{
    __super::Free();
    /*비디오 서비스 통해 플레이어 아이디를 보내서 삭제.*/
    VideoService()->DestroyPlayer(m_PlayerID);
    /*디코더는 직접 삭제*/
    Safe_Release(m_pDecoder);
}
