#pragma once

#include "ICameraService.h"
#include "Camera.h"
#include "ShakeController.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraMgr final : public ICameraService
{
private:
    CCameraMgr() {}
    virtual ~CCameraMgr() DEFAULT;

public:
    void     Update(_float dt)                                    override;

    void     Set_MainCam(CCamera* pCamCom, _float blendSec = 0.f) override;
    void     Set_ShadowCam(CCamera* pCamCom)                      override;

    CCamera* Get_BaseCam()   const override;
    CCamera* Get_ActiveCam() const override;
    CCamera* Get_ShadowCam() const override;

    void     Set_BlendEase(EaseType ease) override { m_easeType = ease; }
    EaseType Get_BlendEase() const        override { return m_easeType; }

    _uint    Push(CCamera* camComp, _float blendTime = 0.25f) override;
    _bool    Pop(_uint handle, _float blendTime = 0.25f)      override;
    void     Clear(_float blendTime = 0.25f)                  override;

public:
    void     SetShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) override;
    void     AddShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) override;
    void     ClearShake(_float fadeOutSec = 0.f)                                       override;

public:
    void     SetShake(CamShakeType type, _float strength = 1.f) override { m_shake.Set(type, strength); }
    void     AddShake(CamShakeType type, _float strength = 1.f) override { m_shake.Add(type, strength); }

    Lens     Get_Lens()       const override;
    Lens     Get_ShadowLens() const override;

public:
    const Matrix* Get_ViewMatrix()               override { return &main.view; }
    const Matrix* Get_ProjMatrix()               override { return &main.proj; }
    const Matrix* Get_InversedViewMatrix()       override { return &main.invView; }
    const Matrix* Get_InversedProjMatrix()       override { return &main.invProj; }
    const Vector4 Get_CameraPos()                override { return main.pos; }
    const _float  Get_Far()                      override { return main.farZ; }

    const Matrix* Get_ShadowViewMatrix()         override { return &shadow.view; }
    const Matrix* Get_ShadowProjMatrix()         override { return &shadow.proj; }
    const Vector4 Get_ShadowCameraPos()          override { return shadow.pos; }
    const _float  Get_ShadowFar()                override { return shadow.farZ; }
    const Matrix* Get_InversedShadowViewMatrix() override { return &shadow.invView; }
    const Matrix* Get_InversedShadowProjMatrix() override { return &shadow.invProj; }

    Vector4 GetForward() const override;
    Vector4 GetRight()   const override;

private:
    struct OverrideEntry
    {
        _uint         handle{};
        OBJECT_HANDLE camObj{};
    };
    struct CamLens
    {
        CamProjType projType = CamProjType::Perspective;

        _float fov{};
        _float nearZ{};
        _float farZ{};
        _float aspect{};
        _float orthoHeight = 10.f;
    };
    struct CamPoseFrame
    {
        Vector3     pos{};
        Quaternion  rot = Quaternion::Identity;
        CamLens     lens{};
    };
    struct CamCache
    {
        Matrix  view = Matrix::Identity;
        Matrix  proj = Matrix::Identity;
        Matrix  invView = Matrix::Identity;
        Matrix  invProj = Matrix::Identity;
        Vector4 pos = {0.f, 0.f, 0.f, 1.f};
        _float  farZ = 0.f;
    };

private:
    CGameObject* ResolveObj(OBJECT_HANDLE handle) const;
    CCamera*     ResolveCam(OBJECT_HANDLE handle) const;

    OBJECT_HANDLE GetActiveCamObj() const { return m_overrides.empty() ? m_baseCamObj : m_overrides.back().camObj; }

private:
    void         SetMainCamObj(OBJECT_HANDLE camObjHandle, _float blendSec);
    void         SetShadowCamObj(OBJECT_HANDLE camObjHandle) { m_shadowCamObj = camObjHandle; }
    _uint        PushCamObj(OBJECT_HANDLE camObjHandle, _float blendSec);

private:
    CamPoseFrame CapturePose(CCamera* cam) const;
    CamPoseFrame BlendPose(const CamPoseFrame& a, const CamPoseFrame& b, _float t) const;

    void         ApplyCache(CamCache& outCache, const CamPoseFrame& pose);
    void         BeginBlendTo(OBJECT_HANDLE targetObj, _float blendSec);

    void         UpdateShadowCache();

private:
    OBJECT_HANDLE   m_baseCamObj{};
    OBJECT_HANDLE   m_shadowCamObj{};
    ShakeController m_shake{};

private:
    vector<OverrideEntry> m_overrides{};
    _uint                 m_nextHandle = 1u;

private:
    _bool         m_isBlending = false;
    _float        m_blendTime{};
    _float        m_blendDuration{};
    CamPoseFrame  m_blendFrom{};
    OBJECT_HANDLE m_blendTargetObj{};

private:
    CamPoseFrame  m_outputPose{};
    EaseType      m_easeType = EaseType::OutSine;

private:
    CamCache main{};
    CamCache shadow{};

public:
    static CCameraMgr* Create() { return new CCameraMgr(); }
    void Free() override;
};

NS_END