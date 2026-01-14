#pragma once
#include "IService.h"
#include "Engine_Math.h"
NS_BEGIN(Engine)
class CCamera; struct Lens;

class ENGINE_DLL ICameraService abstract : public IService
{
protected:
    virtual ~ICameraService() DEFAULT;

public:
    virtual void Update(_float dt) PURE;

    virtual void Set_MainCam(CCamera* pCamCom, _float blendSec = 0.f) PURE;
    virtual void Set_ShadowCam(CCamera* pCamCom) PURE;

    virtual CCamera* Get_BaseCam()   const PURE;
    virtual CCamera* Get_ActiveCam() const PURE;
    virtual CCamera* Get_ShadowCam() const PURE;

    virtual void     Set_BlendEase(EaseType ease) PURE;
    virtual EaseType Get_BlendEase() const PURE;

    virtual _uint Push(CCamera* camComp, _float blendTime = 0.25f) PURE;
    virtual _bool Pop(_uint handle, _float blendTime = 0.25f) PURE;
    virtual void  Clear(_float blendTime = 0.25f) PURE;

public:
    virtual void SetShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) PURE;
    virtual void AddShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) PURE;
    virtual void ClearShake(_float fadeOutSec = 0.f) PURE;

    virtual Lens Get_Lens() const PURE;
    virtual Lens Get_ShadowLens() const PURE;

    virtual const Matrix* Get_ViewMatrix() PURE;
    virtual const Matrix* Get_ProjMatrix() PURE;
    virtual const Matrix* Get_InversedViewMatrix() PURE;
    virtual const Matrix* Get_InversedProjMatrix() PURE;
    virtual const Vector4 Get_CameraPos() PURE;
    virtual const _float  Get_Far() PURE;

    virtual const Matrix* Get_ShadowViewMatrix() PURE;
    virtual const Matrix* Get_ShadowProjMatrix() PURE;
    virtual const Vector4 Get_ShadowCameraPos() PURE;
    virtual const _float  Get_ShadowFar() PURE;
    virtual const Matrix* Get_InversedShadowViewMatrix() PURE;
    virtual const Matrix* Get_InversedShadowProjMatrix() PURE;
};

NS_END
