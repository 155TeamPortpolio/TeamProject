#pragma once
#include "EffectNode.h"

NS_BEGIN(Engine)
class CSpriteNode :
    public CEffectNode
{
private:
    CSpriteNode();
    CSpriteNode(const CSpriteNode& rhs);
    virtual ~CSpriteNode() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CSpriteNode* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();

private:
    _bool m_IsAnimated = false;
    _bool m_IsRepeat = false;
    _float m_fSpeed{};
    _float m_fSpriteElapsedTime{};
    _uint m_iMaxFrameIndex{};
    _uint m_iCurrFrameIndex{};
};
NS_END
