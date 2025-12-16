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
    _float4 m_vColor{};

    EaseType m_eAlphaFadeEase = EaseType::None;
    _float2 m_vAlphaFade{};
    _float m_fAlpha{};

    _float2 m_vUVSpeed{};
    _float2 m_vUVOffset{};

    _uint m_iCol{};
    _uint m_iRow{};
    _uint m_iFrameIndex{};
};
NS_END
