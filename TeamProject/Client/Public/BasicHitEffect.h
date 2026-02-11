#pragma once
#include "EffectContainer.h"

NS_BEGIN(Client)

class CBasicHitEffect :
    public CEffectContainer
{
private:
    CBasicHitEffect();
    CBasicHitEffect(const CBasicHitEffect& rhg);
    virtual ~CBasicHitEffect() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* arg) override;
    void    Awake()                    override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override;

    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr);
    virtual void OnPooledRelease();

public:
    static CBasicHitEffect* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override;

private:

};

NS_END