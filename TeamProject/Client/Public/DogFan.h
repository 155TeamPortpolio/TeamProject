#pragma once
#include "AmbientActor.h"

NS_BEGIN(Client)

class CDogFan final :
    public CAmbientActor
{
private:
    CDogFan();
    CDogFan(const CDogFan& rhs);
    virtual ~CDogFan() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake() override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    static CDogFan* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END