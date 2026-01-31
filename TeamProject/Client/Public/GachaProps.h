#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaProps :
    public CGameObject
{
private:
    CGachaProps();
    CGachaProps(const CGachaProps& rhs);
    virtual ~CGachaProps() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    HRESULT Initialize_GlobalPrototype();
    void Add_GachaProps();

public:
    static CGachaProps* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END