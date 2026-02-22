#include "pch.h"
#include "UI_GachaVideo.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "AudioSource.h"
#include "Sprite2D.h"
#include "MFVideoDecoderBackend.h"

#include "UI_IconButton.h"

HRESULT CUI_GachaVideo::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();
    Add_Component<CAudioSource>();
    Get_Component<CAudioSource>()->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Gacha/Sound/");

    return S_OK;
}

HRESULT CUI_GachaVideo::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    VIDEO_DESC* pDesc = static_cast<VIDEO_DESC*>(pArg);
    m_onVideoFinished = pDesc->onVideoFinished;

    /*비디오를 읽는 디코더를 우선 생성*/
    m_pDecoder = CMFVideoDecoderBackend::Create();

    /*비디오를 플레이할 객체를 요청*/
    m_PlayerID = VideoService()->CreatePlayer(m_pDecoder);
    m_pPlayer = VideoService()->GetPlayer(m_PlayerID);

    /*비디오 플레이어에게 플레이할 영상을 알려줌*/
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = Get_VideoPath(pDesc->eGrade);
    desc.loop = false;
    m_pPlayer->Open(desc);

    VideoService()->StartDecode(m_PlayerID);

    /*아래는 셰이더. 리니어로 샘플링하면 색번짐 생김, 포인트 샘플로로*/
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("VideoPlay");
    m_vSize = { m_WinSize };
     
    return S_OK;
}

void CUI_GachaVideo::Awake()
{
    m_pPlayer->Play();
    Get_Component<CAudioSource>()->Slot("GachaTV.wav").Play();
}

void CUI_GachaVideo::Update(_float dt)
{
    if (m_pPlayer->GetState() == VIDEO_PLAY_STATE::Ended)
    {
        if (m_isFinished)   // 끝나고 한 프레임 뒤에 재생하게
        {
            m_pPlayer->Stop();
            Set_Alive(false);
            return;
        } 

        m_isFinished = true;
        if (m_onVideoFinished)
            m_onVideoFinished(); 
    } 

    __super::Update(dt);

    Get_Component<CSprite2D>()->Set_Param("SpriteTexture", { m_pPlayer->GetSRV(),"Texture2D",0 });

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaVideo::UI_Active(void* pArg)
{ 
}

void CUI_GachaVideo::UI_DeActive(void* pArg)
{ 
}

string CUI_GachaVideo::Get_VideoPath(GachaGrade eGrade)
{
    switch (eGrade)
    {
    case GachaGrade::S: return "../Bin/Resources/Video/GachaS.mp4";
    case GachaGrade::A:
    case GachaGrade::B: return "../Bin/Resources/Video/GachaNormal.mp4";
    default:  return "../Bin/Resources/Video/GachaNormal.mp4";
    }
}

CGameObject* CUI_GachaVideo::Create()
{
    CUI_GachaVideo* pInstance = new CUI_GachaVideo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaVideo::Clone(INIT_DESC* pArg)
{
    CUI_GachaVideo* pInstance = new CUI_GachaVideo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_GachaVideo::Free()
{
    __super::Free();
    /*비디오 서비스 통해 플레이어 아이디를 보내서 삭제.*/
    VideoService()->DestroyPlayer(m_PlayerID);
    /*디코더는 직접 삭제*/
    Safe_Release(m_pDecoder);
}