#pragma once

#include "CamObject.h"
#include "OrbitLockOnController.h"

NS_BEGIN(Client)

class COrbitCam final : public CCamObject
{
public:
    using Profile = OrbitCamProfile;

private:
    COrbitCam() {}
    COrbitCam(const COrbitCam& rhs) : CCamObject(rhs) {}
    virtual ~COrbitCam() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override;
    void    Render_GUI()                override;

public:
    void    SetTarget(OBJECT_HANDLE handle);
    void    ClearTarget();

    void    SyncFromCurTransform();
    void    SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset);
    void    SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot);

    void    CaptureSnapshot(OrbitCamSnapshot& out) const;
    void    RestoreSnapshot(const OrbitCamSnapshot& snapshot);

    void    SetLockOn(OBJECT_HANDLE handle);
    void    ClearLockOn();

    void    SetPivotOverrideOffset(const Vector3& offset) { pose.pivotExternalOffset = offset; }
    Vector3 GetPivotOverrideOffset() const { return pose.pivotExternalOffset; }
    void    ClearPivotOverrideOffset() { pose.pivotExternalOffset = Vector3::Zero; }

    Vector3 GetCurPivotWorld() const { return pose.curPivot; }
    Vector3 GetBasePivotWorld() const { return GetBasePivotTargetPos(targetHandle) + pose.pivotOverrideOffset; }

private:
    void    UpdateInput(_float dt);
    void    ClampTargets();
    void    SmoothStates(_float dt);

    Vector3 GetPivotPos()       const { return pose.curPivot; }
    Vector3 GetPivotTargetPos() const;
    float   GetEffectiveDist()  const;
    void    ApplyOrbitPose(_float dt, const OrbitLockOnEvalResult& lockRes);

    void    UpdateAutoYawFollow(_float dt);
    Vector3 GetTargetFootPos() const;

    Vector3 GetBasePivotTargetPos(OBJECT_HANDLE handle) const;
    void    UpdateTargetSwitch(_float dt);

    _float  GetCollisionAllowedDist(const Vector3& pivot, const Vector3& backDir, float desiredDist);

private:
    OrbitCamPoseState         pose{};
    OrbitCamInputState        input{};
    OrbitCamTargetSwitchState targetSwitch{};
    Profile                   profile{};
    COrbitLockOnController    lockOnCtrl{};
    OBJECT_HANDLE             targetHandle{};

    _float                    autoYawHoldTimer = 0.f;
    Vector3                   prevTargetFoot{};
    _bool                     hasPrevTargetFoot = false;
    _float                    m_curMaxYawSpeedDeg = 720.f;
    _float                    m_curMaxPitchSpeedDeg = 540.f;

public:
    static  COrbitCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END