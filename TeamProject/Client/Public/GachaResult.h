#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaResult :
    public CGameObject
{
private:
    CGachaResult();
    CGachaResult(const CGachaResult& rhs);
    virtual ~CGachaResult() DEFAULT;

public:
    void SetResult(string strModel, string strMaterial, _float4 vRot);

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

public:
    static CGachaResult* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END