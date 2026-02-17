#pragma once
#include "GameObject.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)

class CWaterWave :
    public CGameObject
{
public:
    struct WaterWaveDesc : GAMEOBJECT_DESC {
        _float  fFoamAmount = 1.0f;
        _float  fRoughness = 0.3f;
        _float2 fNoiseOffset = _float2(0.f, 0.f);
        _float3 vWaterTint = _float3(1.f, 1.f, 1.f);
        _float  fTintStrength = 0.f;
        _float  fTsunamiHeight = 30.f;
        _float  fPeakTime = 0.42;
        _float  fRiseWidth = 0.35;
        _float  fFallWidth = 0.12;
        _float  fFadeInEnd = 0.08;
        _float  fFadeOutStart = 0.93;
        _float  fCurlStartRatio = 0.9;
        _float  fCurlDuration = 0.35;
        _float  fWallDepthStart = 0.3;
        _float  fWallDepthEnd = 0.7;
        _float  fCurlForward = 35.f;
        _float  fMaxCurlAngle = 3.456f;
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

public:
    void Check_Attackable(const TsunamiWallDesc& desc);

private:
    void Initialize_Wave(WaterWaveDesc Desc);

private:
    _float      m_fAccTime = 0.f;
    _float3     m_vOriginPos;

     _float     m_CycleTime = 20.f;

     _float     m_CrashMoveDistance = 55.0f;

     _float     m_Roughness;
     _float     m_FoamAmount;
     _float2    m_NoiseOffset;
     _float3    m_WaterTint;
     _float     m_TintStrength;

     _float     m_TsunamiHeight = 30.f;
     _float     m_PeakTime = 0.42;
     _float     m_RiseWidth = 0.35;
     _float     m_FallWidth = 0.12;
     _float     m_FadeInEnd = 0.08;
     _float     m_FadeOutStart = 0.93;
     _float     m_CurlStartRatio = 0.9;
     _float     m_CurlDuration = 0.35;
     _float     m_WallDepthStart = 0.3;
     _float     m_WallDepthEnd = 0.7;
     _float     m_CurlForward= 35.f;
     _float     m_MaxCurlAngle = 3.456f;

     _bool      m_isAttackable = true;
public:
    static CWaterWave* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

NS_END