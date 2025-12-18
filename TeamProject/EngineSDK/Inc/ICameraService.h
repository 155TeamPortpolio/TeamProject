#pragma once
#include "IService.h"
#include "Engine_Math.h"
NS_BEGIN(Engine)
class CCamera;

class ENGINE_DLL ICameraService abstract : public IService
{
protected:
    virtual  ~ICameraService() DEFAULT;
public:
    virtual void             Update(_float dt)                                PURE;
                                                                              
    virtual void             Set_MainCam(CCamera* pCamCom)                    PURE;
    virtual void             Set_ShadowCam(CCamera* pCamCom)                  PURE;

    virtual CCamera*         Get_BaseCam()                              const PURE;
    virtual CCamera*         Get_ActiveCam()                            const PURE;
                                                                              
    virtual void             Set_BlendEase(EaseType ease)                     PURE;
    virtual EaseType         Get_BlendEase()                            const PURE;

    virtual _uint            Push(CCamera* camComp, _float blendTime = 0.25f) PURE;
    virtual _bool            Pop(_uint handle, _float blendTime = 0.25f)      PURE;
    virtual void             Clear(_float blendTime = 0.25f)                  PURE;
                                                                              
    virtual const _float4x4* Get_ViewMatrix()                                 PURE;
    virtual const _float4x4* Get_ProjMatrix()                                 PURE;
    virtual const _float4x4* Get_InversedViewMatrix()                         PURE;
    virtual const _float4x4* Get_InversedProjMatrix()                         PURE;
    virtual const _float4    Get_CameraPos()                                  PURE;
    virtual const _float     Get_Far()                                        PURE;
    virtual const _float4x4* Get_ShadowViewMatrix()                           PURE;
    virtual const _float4x4* Get_ShadowProjMatrix()                           PURE;
    virtual const _float4    Get_ShadowCameraPos()                            PURE;
    virtual const _float     Get_ShadowFar()                                  PURE;
    virtual const _float4x4* Get_InversedShadowViewMatrix()                   PURE;
    virtual const _float4x4* Get_InversedShadowProjMatrix()                   PURE;
};

NS_END
