#include "pch.h"
#include "UI_GachaVideo.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "MFVideoDecoderBackend.h"

#include "UI_IconButton.h"

void CUI_GachaVideo::Play_Video(GachaGrade eGrade)
{
    string filePath = "../Bin/Resources/Video/GachaNormal.mp4";

    switch (eGrade)
    {
    case GachaGrade::S:
        filePath = "../Bin/Resources/Video/GachaS.mp4";
        break;
    case GachaGrade::A:
    case GachaGrade::B:
        filePath = "../Bin/Resources/Video/GachaNormal.mp4";
        break; 
    }

    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = filePath;
    desc.loop = false;
    m_pPlayer->Open(desc);

    UI_Active();
}

HRESULT CUI_GachaVideo::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_GachaVideo::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    /*비디오를 읽는 디코더를 우선 생성*/
    m_pDecoder = CMFVideoDecoderBackend::Create();

    /*비디오를 플레이할 객체를 요청*/
    m_PlayerID = VideoService()->CreatePlayer(m_pDecoder);
    m_pPlayer = VideoService()->GetPlayer(m_PlayerID);

    /*비디오 플레이어에게 플레이할 영상을 알려줌*/
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = "../Bin/Resources/Video/GachaNormal.mp4";
    desc.loop = false;
    m_pPlayer->Open(desc);

    VideoService()->StartDecode(m_PlayerID);

    /*아래는 셰이더. 리니어로 샘플링하면 색번짐 생김, 포인트 샘플로로*/
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("VideoPlay");
    m_vSize = { m_WinSize };

    UI_DeActive();

    return S_OK;
}

void CUI_GachaVideo::Awake()
{
}

void CUI_GachaVideo::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CSprite2D>()->Set_Param("SpriteTexture", { m_pPlayer->GetSRV(),"Texture2D",0 });

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_pPlayer->GetState() == VIDEO_PLAY_STATE::Ended)
        UI_DeActive();
}

void CUI_GachaVideo::UI_Active(void* pArg)
{
    Set_Alive(true);
    VideoService()->StartDecode(m_PlayerID);
    m_pPlayer->Play();
}

void CUI_GachaVideo::UI_DeActive(void* pArg)
{
    Set_Alive(false);
    m_pPlayer->Stop();
    if (m_OnClick)
        m_OnClick();
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