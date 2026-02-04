#pragma once
#include "GachaResult.h"

NS_BEGIN(Client)

class CGachaWeapon :
    public CGachaResult
{
private:
    CGachaWeapon();
    CGachaWeapon(const CGachaWeapon& rhs);
    virtual ~CGachaWeapon() DEFAULT;

public:
    virtual void SetResult(string strModel, string strMaterial, _float4 vRot) override;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    _vector4    m_vInitRot;
    _float      m_fRotElapsedTime = 0.f;
    _float      m_fRotDuration = 0.85f;

public:
    static CGachaWeapon* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END