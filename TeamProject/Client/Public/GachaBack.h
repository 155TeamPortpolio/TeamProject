#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaBack :
    public CGameObject
{
private:
    CGachaBack();
    CGachaBack(const CGachaBack& rhs);
    virtual ~CGachaBack() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    static CGachaBack* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END