#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CWater :
    public CGameObject
{
private:
    CWater();
    CWater(const CWater& rhs);
    virtual ~CWater() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    _float          m_fAccTime = 0.f;

public:
    static CWater* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END