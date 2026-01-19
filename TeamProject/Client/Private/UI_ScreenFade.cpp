#include "pch.h"
#include "UI_ScreenFade.h"

#include "Sprite2D.h"
#include "Engine_Function.h"

HRESULT CUI_ScreenFade::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_ScreenFade::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    pSprite->Change_Texture(0, G_GlobalLevelKey, "empty.png");

    m_vSize = _float2(1600.f, 900.f);
    m_vColor = _float4(0.f, 0.f, 0.f, 1.f);

    Ready_FadeIn();
    Ready_FadeOut();

	return S_OK;
}

void CUI_ScreenFade::Awake()
{
}

void CUI_ScreenFade::Update(_float dt)
{
    __super::Update(dt);
}

void CUI_ScreenFade::Ready_FadeIn()
{
    // 화면 시작할 때 검정 화면이 사라지면서 화면 보임

    UI_ANIM_CLIP clip = {};
    clip.strName = "fadeIn";
    clip.fDuration = 0.5f;
    clip.isLoop = false;

    // --------------------
    // Keyframe 0 (time = 0.0)
    // --------------------
    {
        UI_KEYFRAME keyframe = {};
        keyframe.fTime = 0.0f;
        keyframe.fAngle = 0.0f;
        keyframe.vPosition = { 0.0f, 0.0f };
        keyframe.vScale = { 1.0f, 1.0f };
        keyframe.vColor = { 0.f, 0.f, 0.f, 1.0f }; // alpha 1
        keyframe.easeType = EaseType::InOutSine;

        clip.keyframes.push_back(keyframe);
    }

    // --------------------
    // Keyframe 1 (time = 0.5)
    // --------------------
    {
        UI_KEYFRAME keyframe = {};
        keyframe.fTime = 0.5f;
        keyframe.fAngle = 0.0f;
        keyframe.vPosition = { 0.0f, 0.0f };
        keyframe.vScale = { 1.0f, 1.0f };
        keyframe.vColor = { 0.f, 0.f, 0.f, 0.0f }; // alpha 0
        keyframe.easeType = EaseType::None;

        clip.keyframes.push_back(keyframe);
    }

    m_AnimClips.push_back(clip);
}

void CUI_ScreenFade::Ready_FadeOut()
{
    // 화면 끝날 때 화면이 검정 화면에 가려짐

    UI_ANIM_CLIP clip = {};
    clip.strName = "fadeOut";
    clip.fDuration = 0.5f;
    clip.isLoop = false;

    UI_KEYFRAME keyframe = {};
    // --------------------
    // Keyframe 0 (time = 0.0)
    // --------------------
    {
        UI_KEYFRAME keyframe = {};
        keyframe.fTime = 0.0f;
        keyframe.fAngle = 0.0f;
        keyframe.vPosition = { 0.0f, 0.0f };
        keyframe.vScale = { 1.0f, 1.0f };
        keyframe.vColor = { 0.f, 0.f, 0.f, 0.0f }; // alpha 0
        keyframe.easeType = EaseType::InOutSine;

        clip.keyframes.push_back(keyframe);
    }

    // --------------------
    // Keyframe 1 (time = 0.5)
    // --------------------
    {
        UI_KEYFRAME keyframe = {};
        keyframe.fTime = 0.5f;
        keyframe.fAngle = 0.0f;
        keyframe.vPosition = { 0.0f, 0.0f };
        keyframe.vScale = { 1.0f, 1.0f };
        keyframe.vColor = { 0.f, 0.f, 0.f, 1.0f }; // alpha 1
        keyframe.easeType = EaseType::None;

        clip.keyframes.push_back(keyframe);
    }

    m_AnimClips.push_back(clip);
}

CGameObject* CUI_ScreenFade::Create()
{
    CUI_ScreenFade* pInstance = new CUI_ScreenFade();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ScreenFade");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ScreenFade::Clone(INIT_DESC* pArg)
{
    CUI_ScreenFade* pInstance = new CUI_ScreenFade(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ScreenFade");
        Safe_Release(pInstance);
    }
    return pInstance;
}