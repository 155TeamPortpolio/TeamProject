#include "pch.h"
#include "UI_ScreenFade.h"

#include "Sprite2D.h"
#include "Engine_Function.h"

HRESULT CUI_ScreenFade::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

	return S_OK;
}

HRESULT CUI_ScreenFade::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    pSprite->Change_Texture(0, G_GlobalLevelKey, "empty.png");

    m_vSize = m_WinSize;
    m_vColor = _float4(0.f, 0.f, 0.f, 0.f);

    // 애니메이션 클립 생성하면서 인덱스 캐싱
    Ready_FadeIn();
    Ready_FadeOut();

    m_Zpriority = -999;

	return S_OK;
}

void CUI_ScreenFade::Awake()
{
}

void CUI_ScreenFade::Update(_float dt)
{
    __super::Update(dt);

    if (m_iLastAnimIndex != -1 && Is_AnimFinished())
    {
        if (m_iLastAnimIndex == m_iFadeInIndex)
        {
            if (m_onFadeInFinished)
            {
                m_onFadeInFinished();
                m_onFadeInFinished = nullptr;
                m_iLastAnimIndex = -1;
                return;
            }
        }
        else if (m_iLastAnimIndex == m_iFadeOutIndex)
        {
            if (m_onFadeOutFinished)
            {
                m_onFadeOutFinished();
                m_onFadeOutFinished = nullptr;
                m_iLastAnimIndex = -1;
                return;
            }
        }
    }
}

void CUI_ScreenFade::UI_Active(void* pArg)
{ 
    if (!pArg)
        return;

    FADE_DESC* pDesc = static_cast<FADE_DESC*>(pArg);
    if (!pDesc)
        return;

    if (m_iFadeInIndex < 0)
        return;

    Set_LastKeyframeTime(m_iFadeInIndex, pDesc->fDuration);
    Set_Animation(m_iFadeInIndex);
    m_onFadeInFinished = pDesc->onFinished;
    m_iLastAnimIndex = m_iFadeInIndex;
}

void CUI_ScreenFade::UI_DeActive(void* pArg)
{
    if (!pArg)
        return;

    FADE_DESC* pDesc = static_cast<FADE_DESC*>(pArg);
    if (!pDesc)
        return;

    if (m_iFadeOutIndex < 0)
        return;

    Set_LastKeyframeTime(m_iFadeOutIndex, pDesc->fDuration);
    Set_Animation(m_iFadeOutIndex);
    m_onFadeOutFinished = pDesc->onFinished;
    m_iLastAnimIndex = m_iFadeOutIndex;
}

void CUI_ScreenFade::Ready_FadeIn()
{
    /* 화면 페이드인 실행 (검정 화면 -> 화면) */

    UI_ANIM_CLIP clip = {};
    clip.strName = "fadeIn";
    clip.fDuration = m_fInitDuration;
    clip.isLoop = false;

    clip.keyframes =
    {
        { 0.0f, {1.f,1.f}, 0.f, {}, {0.f,0.f,0.f,1.f}, EaseType::InOutSine },
        { 0.5f, {1.f,1.f}, 0.f, {}, {0.f,0.f,0.f,0.f}, EaseType::None }
    };

    m_iFadeInIndex = Register_AnimClip(clip);
}

void CUI_ScreenFade::Ready_FadeOut()
{
    /* 화면 페이드아웃 실행 (화면 -> 검정 화면) */

    UI_ANIM_CLIP clip = {};
    clip.strName = "fadeOut";
    clip.fDuration = m_fInitDuration;
    clip.isLoop = false;

    clip.keyframes =
    {
        { 0.0f, {1.f,1.f}, 0.f, {}, {0.f,0.f,0.f,0.f}, EaseType::InOutSine },
        { 0.5f, {1.f,1.f}, 0.f, {}, {0.f,0.f,0.f,1.f}, EaseType::None }
    };

    m_iFadeOutIndex = Register_AnimClip(clip);
}

_int CUI_ScreenFade::Register_AnimClip(UI_ANIM_CLIP& clip)
{
    _int iIndex = static_cast<_int>(m_AnimClips.size());
    m_AnimClips.push_back(clip);
    return iIndex;
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