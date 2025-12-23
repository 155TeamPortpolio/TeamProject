#pragma once
#include "ICameraService.h"
#include "Camera.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraMgr final : public ICameraService
{
private:
    CCameraMgr() {}
    virtual ~CCameraMgr() DEFAULT;

public:
    virtual void     Set_MainCam(CCamera* camComp)   override;
    virtual void     Set_ShadowCam(CCamera* camComp) override;

public:
    virtual void     Set_BlendEase(EaseType ease)    override { m_easeType = ease; }
    virtual EaseType Get_BlendEase()           const override { return m_easeType; }                                                   
    CCamera*         Get_BaseCam()             const override { return m_baseCam;  }
    CCamera*         Get_ActiveCam()           const override;

public:
    virtual _uint    Push(CCamera* camComp, _float blendSec = 0.25f) override;
    virtual _bool    Pop(_uint handle, _float blendSec = 0.25f)      override;
    virtual void     Clear(_float blendSec = 0.25f)                  override;

public:
    virtual void SetShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) override;
    virtual void AddShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec = 0.f) override;
    virtual void ClearShake(_float fadeOutSec = 0.f) override;

public:
    virtual void     Update(_float dt) override;

public:
    virtual const Matrix* Get_ViewMatrix()               override { return &m_view;     }
    virtual const Matrix* Get_ProjMatrix()               override { return &m_proj;     }
    virtual const Matrix* Get_InversedViewMatrix()       override { return &m_invView;  }
    virtual const Matrix* Get_InversedProjMatrix()       override { return &m_invProj;  }
    virtual const Vector4 Get_CameraPos()                override { return m_camPos;    }
    virtual const _float  Get_Far()                      override { return m_activeFar; }

    virtual const Matrix* Get_ShadowViewMatrix()         override { return &m_shadowView;    }
    virtual const Matrix* Get_ShadowProjMatrix()         override { return &m_shadowProj;    }
    virtual const Matrix* Get_InversedShadowViewMatrix() override { return &m_shadowInvView; }
    virtual const Matrix* Get_InversedShadowProjMatrix() override { return &m_shadowInvProj; }
    virtual const Vector4 Get_ShadowCameraPos()          override { return m_shadowCamPos;   }
    virtual const _float  Get_ShadowFar()                override { return m_shadowCam ? m_shadowCam->Get_Far() : 0.f; }

private:
    struct OverrideEntry
    {
        _uint    handle{};
        CCamera* cam{};
    };
    struct CamLens
    {
        CamProjType projType = CamProjType::Perspective;

        _float fov;
        _float nearZ;
        _float farZ;
        _float aspect;
        _float orthoHeight = 10.f;
    };
    struct CamPoseFrame
    {
        _vector3   pos{};
        Quaternion rot = Quaternion::Identity;
        CamLens    lens{};
    };
    struct ShakeInstance
    {
        _float  amplitudeDeg{};
        _float  frequency{};
        _float  duration{};
        _float  fadeOutSec{};
        _float  elapsed{};
        Vector3 phase{};
    };

private:
    CCamera* m_baseCam{};
    CCamera* m_shadowCam{};

private:
    vector<OverrideEntry> m_overrides{};
    _uint                 m_nextHandle = 1u;

private:
    _bool        m_isBlending = false;
    _float       m_blendTime{};
    _float       m_blendDuration{};
    CamPoseFrame m_blendFrom{};
    CCamera*     m_blendTargetCam{};

private:
    CamPoseFrame m_outputPose{};
    EaseType     m_easeType = EaseType::OutSine;

private:
    vector<ShakeInstance> m_shakes{};
    _uint                 m_shakeSeed = 1u;

private:
    Matrix  m_view      = Matrix::Identity;
    Matrix  m_proj      = Matrix::Identity;
    Matrix  m_invView   = Matrix::Identity;
    Matrix  m_invProj   = Matrix::Identity;
    Vector4 m_camPos    = { 0.f, 0.f, 0.f, 1.f };
    _float  m_activeFar = 0.f;

private:
    Matrix  m_shadowView    = Matrix::Identity;
    Matrix  m_shadowProj    = Matrix::Identity;
    Matrix  m_shadowInvView = Matrix::Identity;
    Matrix  m_shadowInvProj = Matrix::Identity;
    Vector4 m_shadowCamPos  = { 0.f, 0.f, 0.f, 1.f };

private:
    CamPoseFrame CapturePose(CCamera* cam) const;
    CamPoseFrame GetCurOutputPose() const { return m_outputPose; }

    CamPoseFrame BlendPose(const CamPoseFrame& a, const CamPoseFrame& b, _float t) const;
    void         ApplyOutputPose(const CamPoseFrame& pose);
    void         BeginBlendTo(CCamera* targetCam, _float blendSec);
    void         UpdateShadowCache();
    void         ApplyShake(CamPoseFrame& ioPose, _float dt);

public:
    static CCameraMgr* Create() { return new CCameraMgr(); }
    void Free() override;
};

NS_END