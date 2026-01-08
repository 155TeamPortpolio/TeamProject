#pragma once

#include "CamObject.h"

NS_BEGIN(Client)
enum class OrbitPreset { Field, Battle };

class COrbitCam final : public CCamObject
{
    struct PoseState
    {
        Vector2 targetRotDeg{};
        Vector2 curRotDeg{};

        _float  targetDist{};
        _float  curDist{};

        Vector3 targetPivot{};
        Vector3 curPivot{};

        Vector3 pivotOverrideOffset{};
    };
    struct InputState
    {
        _float sensitivityX = 0.12f;
        _float sensitivityY = 0.08f;
        _float zoomSpeed = 1.0f;
    };

public:
    struct Profile
    {
        _float minDist             = 0.5f;
        _float maxDist             = 6.f;
                  
        _float pitchMin            = -30.f;
        _float pitchMax            =  40.f;
                                   
        _float rotSmoothSpeed      = 12.f;
        _float distSmoothSpeed     = 8.f;
        _float pivotSmoothSpeed    = 15.f;

        _float offsetY             = 0.f;

        _bool  usePitchAutoZoom    = true;
        _float pitchAutoZoomMax    = 0.f;
        _float pitchAutoZoomStartN = 0.75f;
        _float pitchAutoZoomSmooth = 18.f;

        _float startDistance       = 4.5f;
        _float startPitchDeg       = -15.f;
        _float startHeightOffset   = 0.4f;

        _bool  useAutoYawFollow    = true;
        _float autoYawFollowSpeed  = 0.4f;
        _float autoYawFollowDelay  = 0.6f;

        _float collisionZoomInSpeed  = 12.f;
        _float collisionZoomOutSpeed = 6.f;
        _float collisionMinDist      = 1.6f;
    };

private:
    _float  autoYawHoldTimer = 0.f;
    Vector3 prevTargetFoot{};
    _bool   hasPrevTargetFoot = false;

private:


private:
    COrbitCam() {}
    COrbitCam(const COrbitCam& rhs) : CCamObject(rhs) {}
    virtual ~COrbitCam() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Render_GUI()                override;

public:
    void    SetPreset(OrbitPreset nextPreset);
    void    SetTarget(CGameObject* obj);
    void    ClearTarget() { targetHandle.Reset(); }
    void    SyncFromCurTransform();
    void    SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset);
    void    SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot);

private:
    void    UpdateInput(_float dt);
    void    ClampTargets();
    void    SmoothStates(_float dt);

    Vector3 GetPivotPos()       const { return pose.curPivot;}
    Vector3 GetPivotTargetPos() const;
    float   GetEffectiveDist()  const;
    void    ApplyOrbitPose(_float dt);

    void    UpdateAutoYawFollow(_float dt);
    Vector3 GetTargetFootPos() const;

private:
    OrbitPreset       preset = OrbitPreset::Field;
    _bool             firstSnap = true;
    OBJECT_HANDLE     targetHandle{};

    Profile           profile{};
    PoseState         pose{};
    InputState        input{};

    Profile           m_profileInit{};
    InputState        m_inputInit{};
    _bool             m_hasInitSnapshot = false;

public:
    static  COrbitCam*   Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END