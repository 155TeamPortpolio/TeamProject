#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaTV :
    public CGameObject
{
private:
    CGachaTV();
    CGachaTV(const CGachaTV& rhs);
    virtual ~CGachaTV() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    static CGachaTV* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END