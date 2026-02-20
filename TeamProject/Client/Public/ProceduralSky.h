#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CProceduralSky :
    public CGameObject
{
private:
    CProceduralSky();
    CProceduralSky(const CProceduralSky& rhs);
    virtual ~CProceduralSky() DEFAULT;

public:
    void Set_CloudInfo(CLOUD_DESC Desc);
    CLOUD_DESC Get_CloudInfo();

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;
    virtual void    Render_GUI()                override;

private:
    _float          m_fAccTime = 0.f;
    _float3         m_vSunDirection = _float3(0.f, 1.f, 0.f);

    _float3         m_vTopColor = _float3(0.15, 0.3, 0.65);
    _float3         m_vHorizonColor = _float3(0.7, 0.85, 1.0);
    _float          m_fSkyAtmosphereBlend = 0.92;
    _float3         m_vCloudBright = _float3(1.2, 0.5, 0.18);
    _float3         m_vCloudDark = _float3(0.15, 0.06, 0.08);
    _float          m_fCloudCoverage = 0.45;
    _float3         m_vHazeColor = _float3(0.f, 0.f, 0.f);


    Matrix          m_MatProjectionInv;
    Matrix          m_MatViewInv;

public:
    static CProceduralSky* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END