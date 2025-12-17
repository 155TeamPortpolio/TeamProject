#pragma once
#include "EffectNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CMeshNode :
    public CEffectNode
{
protected:
    enum class MODE { UV_ANIMATION, SPRITE_ANIAMTION, END };

protected:
    CMeshNode();
    CMeshNode(const CMeshNode& rhs);
    virtual ~CMeshNode() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CMeshNode* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

protected:    
    MODE m_eMode = MODE::END;
    string m_PassTag{};

    _float4 m_vBaseColor{};
    _float m_fThreshold{};

    /*UV Animation*/
    EaseType m_eAlphaFadeEase = EaseType::None;
    _float2 m_vAlphaFade{};
    _float m_fAlpha{ 1.f };

    EaseType m_eUVEase = EaseType::None;
    _float2 m_vUVSpeed{};
    _float2 m_vStartUVOffset{};
    _float2 m_vEndUVOffset{};
    _float2 m_vCurrUVOffset{};

    /*Sprite Aniamtion*/
    _float m_fSpriteSpeed{};
    _uint m_iCol{};
    _uint m_iRow{};
    _uint m_iMaxFrameIndex{};
    _uint m_iCurrFrameIndex{};

    /*Dissolve Params*/
    _float m_fDissolveThreshold{};
    _float m_fDissolveStartProgress{};

    /*Noise Params*/


    /*Distortion Params*/
};
NS_END
