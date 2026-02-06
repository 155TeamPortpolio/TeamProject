#pragma once
#include "AmbientActor.h"

NS_BEGIN(Client)

class CMilitaryHelicopter final :
    public CAmbientActor
{
private:
    CMilitaryHelicopter();
    CMilitaryHelicopter(const CMilitaryHelicopter& rhs);
    virtual ~CMilitaryHelicopter() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake() override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    static CMilitaryHelicopter* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END