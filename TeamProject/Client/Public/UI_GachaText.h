#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_GachaText final : public CGameObject
{
private:
    CUI_GachaText() {}
    CUI_GachaText(const CUI_GachaText& rhs) : CGameObject(rhs) {}
    virtual ~CUI_GachaText() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override {}

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END