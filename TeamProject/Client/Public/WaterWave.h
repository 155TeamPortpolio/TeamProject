#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CWaterWave :
    public CGameObject
{
public:
    struct WaterWaveDesc : GAMEOBJECT_DESC {
        _float fFoamAmount = 1.0f;
        _float fRoughness = 0.3f;
        _float2 fNoiseOffset = _float2(0.f, 0.f);
        _float3 vWaterTint = _float3(1.f, 1.f, 1.f);
        _float fTintStrength = 0.f;
    };
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
    void Initialize_Wave(WaterWaveDesc Desc);

private:
    _float      m_fAccTime = 0.f;
    _float3     m_vOriginPos;

     _float     m_CycleTime = 7.f;
     _float     m_PeakTime = 0.3f;

     _float     m_CrashMoveDistance = 60.0f;

     _float     m_Roughness;
     _float     m_FoamAmount;
     _float2     m_NoiseOffset;
     _float3    m_WaterTint;
     _float     m_TintStrength;

public:
    static CWaterWave* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

NS_END