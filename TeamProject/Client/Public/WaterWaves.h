#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CWaterWaves :
    public CGameObject
{
private:
    CWaterWaves();
    CWaterWaves(const CWaterWaves& rhs);
    virtual ~CWaterWaves() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    HRESULT         Add_WaterWave();

public:
    static CWaterWaves* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END