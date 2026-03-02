#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaResult :
    public CGameObject
{
protected:
    CGachaResult();
    CGachaResult(const CGachaResult& rhs);
    virtual ~CGachaResult() DEFAULT;

public:
    virtual void SetResult(GACHA_RESULT_DESC Desc) PURE;
    virtual void SetRenderState(_bool Render);

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END