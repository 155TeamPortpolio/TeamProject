#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaFootStage :
    public CGameObject
{
private:
    CGachaFootStage();
    CGachaFootStage(const CGachaFootStage& rhs);
    virtual ~CGachaFootStage() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    static CGachaFootStage* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END