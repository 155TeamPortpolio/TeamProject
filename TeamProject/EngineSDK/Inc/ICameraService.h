#pragma once

#include "IService.h"
#include "Engine_Math.h"
#include "Camera.h"
#include "CamFXData.h"

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
    virtual void RegisterShakePresets(const CamShakePreset* presets, _uint count) PURE;
    virtual void RegisterZoomPresets(const CamZoomPreset* presets, _uint count) PURE;

public:
    virtual void SetShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) PURE;
    virtual void AddShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) PURE;
    virtual void ClearShake(_float fadeOutSec = 0.f) PURE;

    virtual void SetZoomPunch(_float amountDeg, _float attackSec = 0.020f, _float releaseSec = 0.100f) PURE;
    virtual void AddZoomPunch(_float amountDeg, _float attackSec = 0.020f, _float releaseSec = 0.100f) PURE;
    virtual void ClearZoom(_float fadeOutSec = 0.f) PURE;

public:
    virtual void SetShakeType(_uint type, _float strength = 1.f) PURE;
    virtual void AddShakeType(_uint type, _float strength = 1.f) PURE;

    virtual void SetZoomType(_uint type, _float strength = 1.f) PURE;
    virtual void AddZoomType(_uint type, _float strength = 1.f) PURE;

    virtual void AddImpact(_uint shakeType = 0u, _uint zoomType = 0u, _float strength = 1.f) PURE;

public:
    virtual Lens Get_Lens()       const PURE;
    virtual Lens Get_ShadowLens() const PURE;

public:
    virtual const Matrix* Get_ViewMatrix()         PURE;
    virtual const Matrix* Get_ProjMatrix()         PURE;
    virtual const Matrix* Get_InversedViewMatrix() PURE;
    virtual const Matrix* Get_InversedProjMatrix() PURE;
    virtual const Vector4 Get_CameraPos()          PURE;
    virtual const _float  Get_Far()                PURE;

    virtual const Matrix* Get_ShadowViewMatrix()         PURE;
    virtual const Matrix* Get_ShadowProjMatrix()         PURE;
    virtual const Vector4 Get_ShadowCameraPos()          PURE;
    virtual const _float  Get_ShadowFar()                PURE;
    virtual const Matrix* Get_InversedShadowViewMatrix() PURE;
    virtual const Matrix* Get_InversedShadowProjMatrix() PURE;

    virtual Vector4 GetForward() const PURE;
    virtual Vector4 GetRight()   const PURE;
};

NS_END
