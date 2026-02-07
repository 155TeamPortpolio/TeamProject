#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_GachaTextGroup : public CGameObject
{
protected:
    CUI_GachaTextGroup() {}
    CUI_GachaTextGroup(const CUI_GachaTextGroup& rhs) : CGameObject(rhs) {}
    virtual ~CUI_GachaTextGroup() DEFAULT;

public:
    virtual void Show(GachaGrade eGrade);
        
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