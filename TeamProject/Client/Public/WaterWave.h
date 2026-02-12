#pragma once
#include "GameObject.h"

class CWaterWave :
    public CGameObject
{
private:
    CWaterWave();
    CWaterWave(const CWaterWave& rhs);
    virtual ~CWaterWave() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    _float      m_fAccTime = 0.f;
    _float3     m_vOriginPos;

public:
    static CWaterWave* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

