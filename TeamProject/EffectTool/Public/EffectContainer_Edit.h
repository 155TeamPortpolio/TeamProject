#pragma once
#include "EffectContainer.h"

NS_BEGIN(EffectTool)

class CEffectContainer_Edit final:
    public CEffectContainer
{
private:
    CEffectContainer_Edit();
    CEffectContainer_Edit(const CEffectContainer_Edit& rhs);
    virtual ~CEffectContainer_Edit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

public:
    static CEffectContainer_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

NS_END