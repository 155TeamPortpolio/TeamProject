#include "pch.h"
#include "TestVideo.h"
#include "GameInstance.h"
#include "MFVideoDecoderBackend.h"

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

    m_pDecoder = CMFVideoDecoderBackend::Create();
    m_pDecoder->SetLoop(true);
    m_PlayerID = VideoService()->CreatePlayer(m_pDecoder);
    m_pPlayer = VideoService()->GetPlayer(m_PlayerID);
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = "../Bin/Resources/Video/test.mp4"; // 더미는 무시함
    desc.loop = true;

    m_pPlayer->Open(desc);
    m_pPlayer->Play();
    VideoService()->StartDecode(m_PlayerID);
    

    return S_OK;
}

void CTestVideo::Awake()
{
}

void CTestVideo::Update(_float dt)
{
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