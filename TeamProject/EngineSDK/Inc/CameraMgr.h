#pragma once
#include "ICameraService.h"
NS_BEGIN(Engine)
class CCamera;

class CCameraMgr : public ICameraService
{
private:
    CCameraMgr();
    virtual ~CCameraMgr() DEFAULT;
public:
    virtual void Set_MainCam(CCamera* pCamCom) override;
    virtual void Set_ShadowCam(CCamera* pCamCom) override;

public:
    virtual void Update(_float dt) override;

    virtual const _float4x4*  Get_ViewMatrix()               override { return &m_ViewMatrix; };
    virtual const _float4x4*  Get_ProjMatrix()               override { return &m_ProjMatrix; };
    virtual const _float4x4*  Get_InversedViewMatrix()       override { return &m_InversedViewMatrix; };
    virtual const _float4x4*  Get_InversedProjMatrix()       override { return &m_InversedProjMatrix; };
    virtual const _float4     Get_CameraPos()                override;
    virtual const _float      Get_Far()                      override;

    virtual const _float4x4*  Get_ShadowViewMatrix()         override { return &m_ShadowViewMatrix; };
    virtual const _float4x4*  Get_ShadowProjMatrix()         override { return &m_ShadowProjMatrix; };
    virtual const _float4x4*  Get_InversedShadowViewMatrix() override { return &m_ShadowInversedViewMatrix; };
    virtual const _float4x4*  Get_InversedShadowProjMatrix() override { return &m_ShadowInversedProjMatrix; };
    virtual const _float4     Get_ShadowCameraPos()          override;
    virtual const _float      Get_ShadowFar()                override;

public:
    static CCameraMgr* Create();
    virtual void Free() override;

private:
    CCamera*  m_pMainCam{};
    CCamera*  m_pShadowCam{};

    _float4x4 m_ViewMatrix{};
    _float4x4 m_ProjMatrix{};
    _float4x4 m_InversedViewMatrix{};
    _float4x4 m_InversedProjMatrix{};
    _float4   m_vCamPos{};

    _float4   m_vShadowCamPos{};
    _float4x4 m_ShadowViewMatrix{};
    _float4x4 m_ShadowProjMatrix{};
    _float4x4 m_ShadowInversedViewMatrix{};
    _float4x4 m_ShadowInversedProjMatrix{};
};

NS_END