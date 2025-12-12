#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class CCamera;

class ENGINE_DLL ICameraService abstract : public IService
{
protected:
    virtual  ~ICameraService() DEFAULT;
public:
    virtual void             Update(_float dt)               PURE;

    virtual void             Set_MainCam(CCamera* pCamCom)   PURE;
    virtual void             Set_ShadowCam(CCamera* pCamCom) PURE;

    virtual const _float4x4* Get_ViewMatrix()                PURE;
    virtual const _float4x4* Get_ProjMatrix()                PURE;
    virtual const _float4x4* Get_InversedViewMatrix()        PURE;
    virtual const _float4x4* Get_InversedProjMatrix()        PURE;
    virtual const _float4    Get_CameraPos()                 PURE;
    virtual const _float     Get_Far()                       PURE;
    virtual const _float4x4* Get_ShadowViewMatrix()          PURE;
    virtual const _float4x4* Get_ShadowProjMatrix()          PURE;
    virtual const _float4    Get_ShadowCameraPos()           PURE;
    virtual const _float     Get_ShadowFar()                 PURE;
    virtual const _float4x4* Get_InversedShadowViewMatrix()  PURE;
    virtual const _float4x4* Get_InversedShadowProjMatrix()  PURE;
};

NS_END
