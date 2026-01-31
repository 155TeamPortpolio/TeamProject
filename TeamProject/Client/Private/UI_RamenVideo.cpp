#include "pch.h"
#include "UI_RamenVideo.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "MFVideoDecoderBackend.h"

HRESULT CUI_RamenVideo::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_RamenVideo::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    /*비디오를 읽는 디코더를 우선 생성*/
    m_pDecoder = CMFVideoDecoderBackend::Create();

    /*비디오를 플레이할 객체를 요청*/
    m_PlayerID = VideoService()->CreatePlayer(m_pDecoder);
    m_pPlayer = VideoService()->GetPlayer(m_PlayerID);

    /*비디오 플레이어에게 플레이할 영상을 알려줌*/
    CVideoPlayer::VIDEO_PLAYER_DESC desc;
    desc.filePath = "../Bin/Resources/Video/Ramen.mp4";
    desc.loop = false;
    m_pPlayer->Open(desc);

    VideoService()->StartDecode(m_PlayerID);

    /*아래는 셰이더. 리니어로 샘플링하면 색번짐 생김, 포인트 샘플로로*/
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("VideoPlay");
    m_vSize = { 1600 ,900 };

	return S_OK;
}

void CUI_RamenVideo::Awake()
{
}

void CUI_RamenVideo::Update(_float dt)
{
	__super::Update(dt);

    Get_Component<CSprite2D>()->Set_Param("SpriteTexture", { m_pPlayer->GetSRV(),"Texture2D",0 });

    if (m_pPlayer->GetState() == VIDEO_PLAY_STATE::Ended)
        Set_Alive(false);
}

void CUI_RamenVideo::UI_Active(void* pArg)
{
    /*재생 / 디코딩 시작*/
    m_pPlayer->Play(); 
}

void CUI_RamenVideo::UI_DeActive(void* pArg)
{
}

CGameObject* CUI_RamenVideo::Create()
{
    CUI_RamenVideo* pInstance = new CUI_RamenVideo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RamenVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RamenVideo::Clone(INIT_DESC* pArg)
{
    CUI_RamenVideo* pInstance = new CUI_RamenVideo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RamenVideo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_RamenVideo::Free()
{
    __super::Free();
    /*비디오 서비스 통해 플레이어 아이디를 보내서 삭제.*/
    VideoService()->DestroyPlayer(m_PlayerID);
    /*디코더는 직접 삭제*/
    Safe_Release(m_pDecoder);
}