#include "pch.h"
#include "UI_GachaCharacterIntro.h"

#include "GameInstance.h"
#include "Sprite2D.h"
#include "MFVideoDecoderBackend.h"

HRESULT CUI_GachaCharacterIntro::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_GachaCharacterIntro::Initialize(INIT_DESC* pArg)
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
    desc.filePath = "../Bin/Resources/Video/GachaCharacterIntro_Miyabi.mp4";
    desc.loop = false;
    m_pPlayer->Open(desc);

    VideoService()->StartDecode(m_PlayerID);

    /*아래는 셰이더. 리니어로 샘플링하면 색번짐 생김, 포인트 샘플로로*/
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("VideoPlay"); 

    return S_OK;
}

void CUI_GachaCharacterIntro::Awake()
{
    m_vSize = { m_WinSize.x * 0.88f, m_WinSize.y * 0.88f };
    Set_Anchor(ANCHOR::Right | ANCHOR::Center);
    Set_AnchorOffset({ -m_vSize.x, -0.5f * m_vSize.y});
    Set_Alive(false);
}

void CUI_GachaCharacterIntro::Update(_float dt)
{
    __super::Update(dt);
    
    Get_Component<CSprite2D>()->Set_Param("SpriteTexture", { m_pPlayer->GetSRV()  ,"Texture2D",0});
}

void CUI_GachaCharacterIntro::UI_Active(void* pArg)
{ 
    m_pPlayer->Play();
    Set_Alive(true);
}

void CUI_GachaCharacterIntro::UI_DeActive(void* pArg)
{
    m_pPlayer->Stop();
    Set_Alive(false);
}

CGameObject* CUI_GachaCharacterIntro::Create()
{
    CUI_GachaCharacterIntro* pInstance = new CUI_GachaCharacterIntro();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaCharacterIntro");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaCharacterIntro::Clone(INIT_DESC* pArg)
{
    CUI_GachaCharacterIntro* pInstance = new CUI_GachaCharacterIntro(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaCharacterIntro");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_GachaCharacterIntro::Free()
{
    __super::Free();
    /*비디오 서비스 통해 플레이어 아이디를 보내서 삭제.*/
    VideoService()->DestroyPlayer(m_PlayerID);
    /*디코더는 직접 삭제*/
    Safe_Release(m_pDecoder);
}